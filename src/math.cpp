#include <iostream>
#include "../headers/math.hpp"
#include <string.h>
#include <random>

namespace encoding {

    char get_hex_char(char b) {
        if(b >= 0 && b <= 9) {
            return '0' + b;
        }
        return 'a' + (b - 10);
    }

    std::string byte_to_hex(char b) {
        char first_bits = b & 15;
        char second_bits = (b >> 4);
        char arr[2];
        arr[1] = get_hex_char(first_bits);
        arr[0] = get_hex_char(second_bits);
        return std::string{arr};
    }

    void xor_eq_len_buffers(char* b1, char* b2, char* dst, size_t l) {
        for (size_t i = 0; i < l;i++) {
            dst[i] = b1[i] ^ b2[i];
        }
    }

    uint hex_to_dec(std::string hex) {
        uint sum = 0;
        uint pow = 1;
        size_t l = hex.size();
        for (size_t i =  l - 1; i >= 0;i--) {
            //break;
            char c = hex[i];
            //std::cout << i << std::endl;
            //std::cout << "Bjorn" << std::endl;
            if (48 <= c && c <= 57) {
                sum += (c - 48) * pow;
            }
            else {
                sum +=  (10 + c - 65) * pow;
            }
            pow *= 16;
            if (i == 0) {
                break;
            }
        }
        return sum;
    }

    uint pow(uint x, uint n) {
        if (n == 0) {
            return 1;
        }
        if (n % 2 == 1) {
            return x * pow(x * x, (n - 1) / 2);
        }
        return pow(x * x, n / 2);
    }

    char decimal_to_b64(char d) {
        if (0 <= d && d <= 25) {
            return 'A' + d;
        }
        if (26 <= d && d <= 51) {
            return 'a' + (d - 26);
        }
        if (52 <= d && d <= 61) {
            return '0' + (d - 52);
        }
        if (d == 62) {
            return '+';
        }
        return '/';
    }

    //hex_to_byte is a function that takes in a
    //hexadecimal string of length 2 and returns the byte value.
    char hex_to_byte(std::string hex) {
        char c1 = hex[0];
        char c0 = hex[1];
        char sum = 0;
        if (c1 >= '0' && c1 <= '9') {
            sum += 16 * (c1 - '0');
        }
        else {
            sum += 16 * (c1 - 'a' + 10);
        }
        if (c0 >= '0' && c0 <= '9') {
            sum += 1 * (c0- '0');
        }
        else {
            sum += 1 * (c0 - 'a' + 10);
        }
        return sum;
    }

    std::string hex_string_to_bytes(std::string h) {
        size_t l = h.size();
        if (l % 2 != 0) {
            l++;
            h = "0" + h;
        }
        char buf[l / 2];
        for (size_t i = 0; i < l;i += 2) {
            std::string sub = h.substr(i, 2);
            buf[i / 2] = hex_to_byte(sub);
        }
        std::string t = std::string{buf};
        return t;
    }

    //hex_string_to_bytes takes in a hex string, a buffer and how much
    //of the hex string is to be converted to bytes.
    void hex_string_to_bytes(std::string h, char *buf, size_t l) {
        for (size_t i = 0; i < l;i += 2) {
            std::string sub = h.substr(i, 2);
            buf[i / 2] = hex_to_byte(sub);
        }
    }

    std::string bytes_to_hex(char *buf) {
        std::string ret = "";
        size_t l = strlen(buf);
        for (size_t i = 0; i < l; i++) {
            std::string h = byte_to_hex(buf[i]);
            ret = ret + h;
        }
        return ret;
    }

    std::string bytes_to_hex(std::string buf) {
        std::string ret = "";
        size_t l = buf.size();
        for (size_t i = 0; i < l; i++) {
            std::string h = byte_to_hex(buf[i]);
            ret = ret + h;
        }
        return ret;
    }

    RandomGenerator::RandomGenerator() {
        srand((unsigned)time(0));
    }
    RandomGenerator::~RandomGenerator() {}
    int RandomGenerator::generate_random_num(int max, int min) {
        return min + (rand() % (max + 1));
    }
    char RandomGenerator::generate_random_byte() {
        return (char) this->generate_random_num(255, -128);
    }

    std::string RandomGenerator::random_pad_string(std::string s) {
        std::string tmp = s;
        int left_padding = this->generate_random_num(5, 5);
        int right_padding = this->generate_random_num(5, 5);
        for (int i = 0; i < right_padding;i++) {
            tmp.push_back(this->generate_random_byte());
        }
        std::string t;
        for (int j = 0; j < left_padding;j++) {
            t.push_back(this->generate_random_byte());
        }
        return t + tmp;
    }

    std::string RandomGenerator::random_prefix_string(std::string s, int max_prefix) {
        int prefix_len = this->generate_random_num(max_prefix, 0);
        std::string t = s;
        for (int i = 0; i < prefix_len;i++) {
            t.insert(0, 1, this->generate_random_byte());
        }
        return t;
    }

    char *RandomGenerator::random_key(size_t l) {
        char *key = (char*) malloc(l);
        for (size_t i = 0; i < l;i++) {
            key[i] = this->generate_random_byte();
        }
        return key;
    }
    char *RandomGenerator::random_IV(size_t l)  {
        return this->random_key(l);
    }
}