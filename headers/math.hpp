#ifndef MATH_HPP
#define MATH_HPP
#include <iostream>
namespace encoding {
    uint hex_to_dec(std::string hex);
    uint pow(uint x, uint n);
    char decimal_to_b64(char d);
    char hex_to_byte(std::string hex);
    std::string hex_string_to_bytes(std::string h);
    void xor_eq_len_buffers(char* b1,char* b2, char* dst, size_t l);
    std::string byte_to_hex(char b);
    std::string bytes_to_hex(char *buf);
    std::string bytes_to_hex(std::string buf);
    void hex_string_to_bytes(std::string h, char *buf, size_t l);
    int generate_random_num(int max, int min);
    char generate_random_byte();

    class RandomGenerator {
        public:
            RandomGenerator();
            ~RandomGenerator();
            int generate_random_num(int max, int min);
            char generate_random_byte();
            char *generate_random_key(size_t l);
            char *random_key(size_t l);
            char *random_IV(size_t l);
            std::string random_pad_string(std::string s);
    };
}
#endif