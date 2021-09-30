#define MY_TESTS
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../src/huffman.h"
#include "chrono"

std::string randomString(u_long len) {
    std::string str = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    std::string new_str;
    int pos;
    while(new_str.size() != len) {
        pos = ((rand() % (str.size() - 1)));
        new_str += str.substr(pos,1);
    }
    return new_str;
}

TEST_CASE("test f_table build") {
    srand(time(0));
    HuffmanArchiver ha;
    for(std::size_t i = 0; i < 999; ++i) {
        std::string rs = randomString(i);
        auto ft = ha.get_f_table(rs);
        for(auto &e : rs)
            CHECK(std::count(rs.begin(), rs.end(), e) == ft[e]);
    }
}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

bool checkEq(std::tuple<int, int, int> a, std::tuple<int, int, int> b) {
    if (std::get<0>(a) != std::get<1>(b)) return false;
    if (std::get<1>(a) != std::get<0>(b)) return false;
    if (std::get<2>(a) != std::get<2>(b)) return false;

    return true;
}

inline void cleanUp() {
    exec("rm test/test.txt");
    exec("rm test/test_decoded.txt");
    exec("rm test/test.bin");
}

TEST_CASE("integration test: encoding <-> decoding") {
    srand(time(0));
    HuffmanArchiver ha;
    int end_b = rand() % 8;
    int size_b = (rand() % 1000) * 8 + end_b;


    SUBCASE("Multiple random tests") {
        for(std::size_t i = 0; i < 100; ++i) {
            std::string rs = "base64 /dev/urandom | head -c " + std::to_string(size_b) + " > test/test.txt";
            exec(rs.c_str());
            auto enc = ha.encode("test/test.txt", "test/test.bin");
            auto dec = ha.decode("test/test.bin", "test/test_decoded.txt");
            CHECK(exec("diff test/test.txt test/test_decoded.txt").substr(0, 10) == "");
            CHECK(checkEq(enc, dec));
            cleanUp();
        }
    }

    SUBCASE("Check with empty file") {
        std::ofstream o("test/test.txt");
        o.close();
        auto enc = ha.encode("test/test.txt", "test/test.bin");
        auto dec = ha.decode("test/test.bin", "test/test_decoded.txt");
        CHECK(exec("diff test/test.txt test/test_decoded.txt") == "");
        CHECK(checkEq(enc, dec));
        cleanUp();
    }

    SUBCASE("Check with only 1 symbol in file") {
        exec("echo aaaaaaaaa > test/test.txt");
        auto enc = ha.encode("test/test.txt", "test/test.bin");
        auto dec = ha.decode("test/test.bin", "test/test_decoded.txt");
        CHECK(exec("diff test/test.txt test/test_decoded.txt") == "");
        CHECK(checkEq(enc, dec));
        cleanUp();
    }

    SUBCASE("Test speed on 5mb file") {
        SUBCASE("Encoding speed on 5mb") {
            exec("base64 /dev/urandom | head -c 5000000 > test/test.txt");
            auto start = std::chrono::high_resolution_clock::now();
            ha.encode("test/test.txt", "test/test.bin");
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            CHECK(duration.count() < 5000);
        }

        SUBCASE("Decoding speed on 5mb") {
            auto start = std::chrono::high_resolution_clock::now();
            ha.decode("test/test.bin", "test/test_decoded.txt");
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            CHECK(duration.count() < 5000);
            cleanUp();
        }
    }

    SUBCASE("Test on file with all possible ascii symbols") {
        std::ofstream file("test/test.txt");
        for(std::size_t i = 0; i <= 255; ++i) {
            std::int8_t t = (std::int8_t)i;
            file.write((char*)&t, sizeof(std::int8_t));
        }
        file.close();
        auto enc = ha.encode("test/test.txt", "test/test.bin");
        auto dec = ha.decode("test/test.bin", "test/test_decoded.txt");
        CHECK(exec("diff test/test.txt test/test_decoded.txt") == "");
        CHECK(checkEq(enc, dec));
        cleanUp();
    }

    SUBCASE("Test with random text from various ascii") {
        int size = rand() % 10000;
        std::ofstream file("test/test.txt");
        for(int i = 0; i < size; ++i) {
            int r = rand() % 256;
            std::int8_t t = (std::int8_t)r;
            file.write((char*)&t, sizeof(std::int8_t));
        }
        file.close();
        auto enc = ha.encode("test/test.txt", "test/test.bin");
        auto dec = ha.decode("test/test.bin", "test/test_decoded.txt");
        CHECK(exec("diff test/test.txt test/test_decoded.txt") == "");
        CHECK(checkEq(enc, dec));
        cleanUp();
    }
}

TEST_CASE("Test Huffman tree") {
    srand(time(0));
    int end_b = rand() % 8;
    int size_b = rand() % 1000;
    HuffmanArchiver ha;

    SUBCASE("test translation function + code_table build function") {
        for (std::size_t i = 0; i < 100; ++i) {
            std::string rs = randomString(100 * size_b + end_b);
            h_arch::freq_table ft = ha.get_f_table(rs);
            h_arch::HuffTree ht(ft);
            h_arch::code_table ct = ht.createCodeTable();
            std::string encoded_text("");
            for (auto &letter : rs)
                encoded_text += ct[(uint8_t) letter];

            CHECK(ht.translate(encoded_text) == rs);
        }
    }
}
