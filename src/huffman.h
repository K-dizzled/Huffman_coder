#ifndef LAB_08_HUFFMAN_ARCHIVER_H
#define LAB_08_HUFFMAN_ARCHIVER_H

#include "HuffTree.h"
#include "fstream"
#include "cmath"
#include "tuple"
#include "cstdint"
#include "bitset"

typedef std::tuple<int, int, int> arch_info;

class HuffmanArchiver {
private:

    #ifdef MY_TESTS
    public:
    #endif

    const int _byte_ = 8;
    const int log_8 = 3;
    h_arch::freq_table f_Table;

    template<typename T>
    void ofWrite(std::ofstream& file, T& value) const {
        file.write((char*)&value, sizeof(T));
    }

    inline int read_int_from_file(std::ifstream& file) const {
        int32_t val;
        file.read((char*)&val, sizeof(int));
        return val;
    }

    inline std::string char_to_binary(const char& ch) const;
    std::string buildFreqTable(std::ifstream& file);
    int fwrite_c(std::ofstream &out_file, const std::size_t textSize, const std::string encoded_text);
    std::string string_to_binary(std::string encoded_text_ch, int end_bytes) const;
    std::string readCode(std::ifstream& file) const;
    h_arch::freq_table get_f_table(std::string text);

public:
    HuffmanArchiver();
    arch_info encode(std::string inputFile, std::string outputFile);
    arch_info decode(std::string inputFile, std::string outputFile);

};

class HuffmanArchiverException : public std::exception {
private:
    std::string what_message;

public:
    explicit HuffmanArchiverException(const char* message)
            : what_message(message) {}

    const char* what() const noexcept override {
        return what_message.c_str();
    }

};

#endif