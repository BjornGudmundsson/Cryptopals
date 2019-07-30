#ifndef VIGENERE_HPP
#define VIGENERE_HPP
#include <string>
namespace Vigenere {
    //repeating_XOR can be used to both decrypt and encrypt a string using the vigenere cipher.
    std::string repeating_XOR(char *buf, char *key, size_t key_size, size_t buffer_size);
    std::string repeating_XOR(const char *buf, const char *key, size_t key_size, size_t buffer_size);
    std::string repeating_XOR(std::string buf,std::string key, size_t key_size);
    size_t hamming_distance(char b1, char b2);
    size_t hamming_distance(std::string s1, std::string s2);
    std::string break_repeating_key_XOR(std::string ct);
}
#endif