#ifndef FREQUENCY_HPP
#define FREQUENCY_HPP
#include <string>
namespace frequency {
    double char_to_score(char c);
    void xor_buffer_to_byte(char *buf, char key, size_t l);
    double score_string(const char *buf, int l);
    std::string xor_buffer_to_byte_to_string(std::string buf, char key, size_t l);
    char detect_single_byte_xor(std::string ct);
}
#endif