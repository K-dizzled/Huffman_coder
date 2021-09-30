#include "huffman.h"

bool is_empty(std::ifstream& pFile) {
    return pFile.peek() == std::ifstream::traits_type::eof();
}

HuffmanArchiver::HuffmanArchiver() {}

std::string HuffmanArchiver::char_to_binary(const char& ch) const {
    std::uint8_t integer_form = (std::uint8_t)ch;
    std::string binary;
    std::uint8_t mask = 1;
    for(int i = 0; i < _byte_; i++) {
        if((mask & integer_form) >= 1)
            binary = "1" + binary;
        else
            binary = "0" + binary;

        mask <<= 1;
    }

    return binary;
}

std::string HuffmanArchiver::buildFreqTable(std::ifstream& file) {
    std::string text;
    try {
        file.seekg(0, std::ios::end);
        text.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
        text.assign((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
    } catch (std::exception& e) {
        std::string a = "ERROR! Error while attempting to read the file:\n";
        a += e.what();
        throw HuffmanArchiverException(a.c_str());
    }
    f_Table = get_f_table(text);
    return text;
}

h_arch::freq_table HuffmanArchiver::get_f_table(std::string text) {
    h_arch::freq_table ft{};
    for(std::size_t i = 0; i < text.size(); ++i)
        ft[(uint8_t)(text[i])]++;

    return ft;
}

int HuffmanArchiver::fwrite_c(std::ofstream &out_file, const std::size_t textSize, const std::string encoded_text) {
    int back_bytes = (_byte_ - (textSize % _byte_)) % _byte_;
    ofWrite(out_file, back_bytes);
    for(std::size_t i = 0; i < textSize / _byte_; ++i) {
        int tmp;
        tmp = std::stoi(encoded_text.substr((i << log_8), _byte_), nullptr, 2);
        out_file.write((char*)&tmp, 1);
    }

    if(back_bytes > 0) {
        auto tmp = (std::stoi(encoded_text.substr(textSize - textSize % _byte_, textSize % _byte_),
                              nullptr,
                              2) << (_byte_ - textSize % _byte_));

        out_file.write((char *) &tmp, 1);
    }

    return back_bytes;
}

std::string HuffmanArchiver::readCode(std::ifstream& file) const {
    auto end_of_table_pos = file.tellg();

    std::string encoded_text_ch;
    try {
    file.seekg(0, std::ios::end);
    encoded_text_ch.reserve(file.tellg() - end_of_table_pos);
    file.seekg(end_of_table_pos, std::ios::beg);
    encoded_text_ch.assign((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
    } catch (std::exception& e) {
        std::string a = "ERROR! Error while attempting to read the file:\n";
        a += e.what();
        throw HuffmanArchiverException(a.c_str());
    }

    return encoded_text_ch;
}

std::string HuffmanArchiver::string_to_binary(std::string encoded_text_ch, int end_bytes) const {
    std::size_t size = encoded_text_ch.size();
    std::string encoded_text_bin ((size << log_8) - end_bytes, '\0');
    std::string byte;
    for(std::size_t i = 0; i < size - 1; ++i) {
        byte = char_to_binary(encoded_text_ch[i]);
        for(std::size_t j = (i << log_8); j < ((i + 1) << log_8); ++j)
            encoded_text_bin[j] = byte[j % _byte_];
    }

    byte = char_to_binary(encoded_text_ch[size - 1]);
    for(std::size_t i = 0; i < (std::size_t)(_byte_ - end_bytes); ++i)
        encoded_text_bin[((size - 1) << log_8) + i] = byte[i];

    return encoded_text_bin;
}

arch_info HuffmanArchiver::encode(std::string inputFile, std::string outputFile) {
    std::ifstream file(inputFile);
    if (!file)
        throw HuffmanArchiverException("LOAD: unable to open input file.");

    if(is_empty(file)) {
        file.close();
        std::ofstream tmp(outputFile);
        tmp.close();
        return std::make_tuple(0, 0, 0);
    }

    std::string text = buildFreqTable(file);
    file.close();

    h_arch::HuffTree huffmanTree(f_Table);
    h_arch::code_table ct = huffmanTree.createCodeTable();

    std::size_t memory_counter = 0;
    std::string encoded_text("");
    for(auto &letter : text) {
        encoded_text += ct[(uint8_t)letter];
        memory_counter += ct[(uint8_t)letter].size();
    }

    std::ofstream out_file(outputFile, std::ios::binary);
    if (!out_file)
        throw HuffmanArchiverException("LOAD: unable to open output file.");

    std::size_t textSize = encoded_text.size();
    int ft_size = huffmanTree.get_f_table_size();
    ofWrite(out_file, ft_size);

    for(std::size_t i = 0; i < huffmanTree.ascii_variety; ++i) {
        if(f_Table[i] != 0) {
            char y = (char)i;
            ofWrite(out_file, y);
            int t = f_Table[i];
            out_file.write((char*)&t, sizeof(int));
        }
    }

    fwrite_c(out_file, textSize, encoded_text);
    out_file.close();

    return std::make_tuple(text.size(),
                           std::ceil((float)memory_counter / 8.0),
                           ft_size * (sizeof(int) + 1) + 2 * sizeof(int));

}

arch_info HuffmanArchiver::decode(std::string inputFile, std::string outputFile) {
    std::ifstream file(inputFile, std::ios::binary);
    if (!file)
        throw HuffmanArchiverException("LOAD: unable to open input file.");

    if(is_empty(file)) {
        file.close();
        std::ofstream tmp(outputFile);
        tmp.close();
        return std::make_tuple(0, 0, 0);
    }

    h_arch::freq_table ft{};
    int ft_size = read_int_from_file(file);
    char* ch = new char;
    for(int i = 0; i < ft_size; ++i) {
        file.read(ch, 1);
        ft[(uint8_t)*ch] = read_int_from_file(file);
    }
    delete ch;
    h_arch::HuffTree huffmanTree(ft);

    std::ofstream output_file(outputFile);
    if (!output_file)
        throw HuffmanArchiverException("LOAD: unable to open input file.");

    int end_bytes = read_int_from_file(file);
    std::string encoded_text_ch = readCode(file);
    file.close();

    std::string encoded_text_bin = string_to_binary(encoded_text_ch, end_bytes);
    std::string decoded = huffmanTree.translate(encoded_text_bin);

    output_file.write(decoded.c_str(), decoded.length());
    output_file.close();

    return std::make_tuple(std::ceil((float)encoded_text_bin.size() / 8.0),
                           decoded.size(),
                           ft_size * (sizeof(int) + 1) + 2 * sizeof(int));
}