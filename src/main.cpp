#include "huffman.h"

std::tuple<int, std::string, std::string> checkParams(int argc, char** argv) {
    if(argc != 6)
        throw HuffmanArchiverException("ERROR! Incorrect parameters amount");

    int type = -1;
    std::string inp, out;
    try {
        for(std::size_t i = 1; i < 6; ++i) {
            if(strcmp(argv[i], "-c") == 0)
                type = 1;
            if(strcmp(argv[i], "-u") == 0)
                type = 0;
            if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0)
                inp += argv[i + 1];
            if(strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0)
                out += argv[i + 1];
        }
    } catch (std::exception& e) {
        std::string a = "ERROR! Invalid params format:\n";
        a += e.what();
        throw HuffmanArchiverException(e.what());
    }
    if(type == -1 || inp == "" || out == "")
        throw HuffmanArchiverException("ERROR! Invalid params format");

    return std::make_tuple(type, inp, out);
}

int main(int argc, char** argv) {
    HuffmanArchiver ha;
    std::tuple<int, std::string, std::string> params;
    bool flag = false;

    try{
        params = checkParams(argc, argv);
    } catch (HuffmanArchiverException& e) {
        std::cout << e.what();
        flag = true;
    }

    if(!flag) {
        if(std::get<0>(params) == 1) {
            try {
                auto a = ha.encode(std::get<1>(params), std::get<2>(params));
                std::cout << std::get<0>(a) << '\n'
                          << std::get<1>(a) << '\n'
                          << std::get<2>(a) << '\n';
            } catch (HuffmanArchiverException& e) {
                std::cout << e.what() << '\n';
            }
        } else {
            try {
                auto a = ha.decode(std::get<1>(params), std::get<2>(params));
                std::cout << std::get<0>(a) << '\n'
                          << std::get<1>(a) << '\n'
                          << std::get<2>(a) << '\n';
            } catch (HuffmanArchiverException& e) {
                std::cout << e.what() << '\n';
            }
        }
    }

    return 0;
}
