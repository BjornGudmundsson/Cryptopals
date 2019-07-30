#include <string>
#include <vector>
#include "../headers/b64.hpp"
#include "../headers/math.hpp"
#include <algorithm>

namespace base64 {
    
    std::string bytes_to_b64(char b1, char b2, char b3) {
        char bits_first = (b1 >> 2);
        char first = encoding::decimal_to_b64(bits_first);
        char bits_2nd = ((b1 & 3) << 4) ^ (b2 >> 4);
        char second = encoding::decimal_to_b64(bits_2nd);
        char bits_3rd = ((b2 & 15) << 2) ^ (b3 >> 6);
        char third = encoding::decimal_to_b64(bits_3rd);
        char bits_4th = (b3 & 63);
        char fourth = encoding::decimal_to_b64(bits_4th);
        char arr[4];
        arr[0] = first;
        arr[1] = second;
        arr[2] = third;
        arr[3] = fourth;
        std::string ret = std::string{arr};
        return ret;
    }
    
    std::string hex_to_b64(std::string h) {
        size_t l = h.size();
        if (l % 2 == 1) {
            h = "0" + h;
        }
        std::vector<char> byte_array;
        for (size_t i = 0; i < l;i += 2) {
            std::string sub = h.substr(i, 2);
            char b = encoding::hex_to_byte(sub);
            byte_array.push_back(b);
        }
        size_t vec_size = byte_array.size();
        size_t padding = 0;
        if (vec_size % 3 != 0) {
            padding = 3 - (vec_size % 3);
            for (size_t i = 0; i < padding;i++) {
                //Pad with null bytes.
                byte_array.push_back('\0');
            }
            vec_size += padding;
        }
        std::string ret = "";
        for (size_t i = 0; i < vec_size;i += 3) {
            char b1 = byte_array[i];
            char b2 = byte_array[i + 1];
            char b3 = byte_array[i + 2];
            std::string tmp = bytes_to_b64(b1, b2, b3);
            ret = ret + tmp;
        }
        for (size_t i = 0; i < padding;i++) {
            ret[ret.size() - 1 - i] = '=';
        }
        return ret;
    }

    std::string b64_chars_to_bytes(char b1, char b2, char b3, char b4) {
        std::string ret = "";
        char b1prime = (b1 << 2) ^ (b2 >> 4);
        char b2prime = (b2 << 4) ^ (b3 >> 2);
        char b3prime = (b3 << 6) ^ b4;
        ret.push_back(b1prime);
        ret.push_back(b2prime);
        ret.push_back(b3prime);
        return ret;
    }

    char b64_char_to_byte(char b) {
        if ('A' <= b && b <= 'Z') {
            return b - 'A';
        }
        if ('a' <= b && b <= 'z') {
            return 26 + (b - 'a');
        }
        if ('0' <= b && b <= '9') {
            return 52 + (b - '0');
        }
        if (b == '+') {
            return 62;
        }
        if (b == '=') {
            return '\0';
        }
        return 63;
    }

    std::string b64_to_bytes(std::string b64) {
        std::string ret = "";
        size_t l = b64.size();
        size_t occurences_of_eq = std::count(b64.begin(), b64.end(), '=');
        for (size_t i = 0;i < l;i += 4) {
            std::string sub = b64.substr(i, 4);
            std::string temp = "";
            for (char c: sub) {
                temp.push_back(b64_char_to_byte(c));
            }
            std::string byte_sub = b64_chars_to_bytes(temp[0], temp[1], temp[2], temp[3]);
            ret += byte_sub;
        }
        size_t l2 = ret.size();
        return ret.substr(0, l2 - occurences_of_eq);
    }
}