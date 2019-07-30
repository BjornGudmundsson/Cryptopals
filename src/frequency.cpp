#include <string>
namespace frequency
{
    double char_to_score(char c) {
        if (c == 'E' || c == 'e') {
            return 12.02;
        }
        if (c == 'T' || c == 't') {
            return 9.10;
        }
        if (c == 'A' || c == 'a') {
            return 8.12;
        }
        if (c == 'O' || c == 'o') {
            return 7.68;
        }
        if (c == 'I' || c == 'i') {
            return 7.31;
        }
        if (c == 'N' || c == 'n') {
            return 6.95;
        }
        if (c == 'S' || c == 's') {
            return 6.28;
        }
        if (c == 'R' || c == 'r') {
            return 6.02;
        }
        if (c == 'H' || c == 'h') {
            return 5.92;
        }
        if (c == 'D' || c == 'd') {
            return 4.32;
        }
        if (c == 'L' || c == 'l') {
            return 3.98;

        }
        if (c == 'U' || c == 'u') {
            return 2.88;
        }
        if (c == 'C' || c == 'c') {
            return 2.71;
        }
        if (c == 'M' || c == 'm') {
            return 2.61;
        }
        if (c == 'F' || c == 'f') {
            return 2.30;
        }
        if (c == 'Y' || c == 'y') {
            return 2.11;
        }
        if (c == 'W' || c == 'w') {
            return 2.09;
        }
        if (c == 'G' || c == 'g') {
            return 2.03;
        }
        if (c == 'P' || c == 'p') {
            return 1.82;
        }
        if (c == 'B' || c == 'b') {
            return 1.49;
        }
        if (c == 'V' || c == 'v') {
            return 1.11;
        }
        if (c == 'K' || c == 'k') {
            return 0.69;
        }
        if (c == 'X' || c == 'x') {
            return 0.17;
        }
        if (c == 'Q' || c == 'q') {
            return 0.11;
        }
        if (c == 'J' || c == 'j') {
            return 0.10;
        }
        if (c == 'Z' || c == 'z') {
            return 0.07;
        }
        if (c == ' ') {
            return 15.0;
        }
        return 0.0;
    }

    void xor_buffer_to_byte(char *buf, char key, size_t l) {
        for (size_t i = 0; i < l;i++) {
            buf[i] = buf[i] ^ key;
        }
    }

    double score_string(const char *buf, int l) {
        double score = 0;
        for (int i = 0; i < l;i++) {
            score += char_to_score(buf[i]);
        }
        return score;
    }

    std::string xor_buffer_to_byte_to_string(std::string buf, char key, size_t l) {
        for (size_t i = 0; i < l;i++) {
            buf[i] = buf[i] ^ key;
        }
        return buf;
    }
    char detect_single_byte_xor(std::string ct) {
        size_t l = ct.size();
        double max = 0;
        char max_key = '\0';
        for (char i = -128; i < 127;i++) {
            std::string tmp = frequency::xor_buffer_to_byte_to_string(ct, i, l);
            double score = frequency::score_string(tmp.c_str(), tmp.size());
            if (score > max) {
                max = score;
                max_key = i;
            }
        }
        char last = 127;
        std::string tmp = frequency::xor_buffer_to_byte_to_string(ct, last, l);
        double score = frequency::score_string(tmp.c_str(), tmp.size());
        if (score > max) {
            max = score;
            max_key = last;
        }
        return max_key;
    }
} // namespace frequency
