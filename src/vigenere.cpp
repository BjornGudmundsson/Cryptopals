#include "string.h"
#include "../headers/vigenere.hpp"
#include "../headers/frequency.hpp"
#include <iostream>
#include <vector>
#include <limits>  

namespace Vigenere {
    const int start_key_size = 2;
    const int end_key_size = 40;
    std::string repeating_XOR(const char *buf, const char *key, size_t key_size, size_t buffer_size) {
        std::string buffer{buf};
        for (size_t i = 0; i < buffer_size;i++) {
            buffer[i] = buf[i] ^ key[i % key_size];
        }
        return buffer;
    }

    std::string repeating_XOR(char *buf,char *key, size_t key_size, size_t buffer_size) {
        std::string buffer{buf};
        for (size_t i = 0; i < buffer_size;i++) {
            buffer[i] = buf[i] ^ key[i % key_size];
        }
        return buffer;
    }

    std::string repeating_XOR(std::string buf,std::string key, size_t key_size) {
        std::string buffer = "";
        size_t buffer_size = buf.size();
        for (size_t i = 0; i < buffer_size;i++) {
            buffer.push_back(buf[i] ^ key[i % key_size]);
        }
        return buffer;
    }

    size_t hamming_distance(char b1, char b2) {
        char b3 = b1 ^ b2;
        size_t dst = 0;
        for (char i = 0; i < 8;i++) {
            size_t bit = b3 & 1;
            dst += bit;
            b3 = b3 >> 1;
        }
        return dst;
    }
    size_t hamming_distance(std::string s1, std::string s2) {
        size_t l1 = s1.size();
        size_t l2 = s2.size();
        size_t max = l1 > l2 ? l1 : l2;
        size_t min = l1 < l2 ? l1 : l2;
        size_t dst = 0;
        dst += (max - min) * 8;
        for (size_t i = 0; i < min;i++) {
            size_t dst_index = hamming_distance(s1[i], s2[i]);
            dst += dst_index;
        }
        return dst;

    }

    std::string break_repeating_key_XOR(std::string ct) {
        int max = std::numeric_limits<int>::max();
        double min_dsts[] = {1.0 * max, 1.0 * max, 1.0 * max};

        int key_sizes[] = {0, 0, 0};
        int guess_count = 3;
        size_t l = ct.size();
        for (int i = start_key_size; i <= end_key_size;i++) {
            std::string block1 = ct.substr(0, i);
            std::string block2 = ct.substr(i, i);
            std::string block3 = ct.substr(2 * i, i);
            double dst1 = (1.0 * hamming_distance(block1, block2)) / i;
            double dst2 = (1.0 * hamming_distance(block1, block3)) / i;
            double dst3 = (1.0 * hamming_distance(block3, block2)) / i;
            double dst = (dst1 + dst2 + dst3) / 3;
            //std::cout << "Distance: " << dst <<std::endl;
            if (dst < min_dsts[0]) {
                min_dsts[0] = dst;
                key_sizes[0] = i;
                //Sorting the array
                for (int j = 0; j < guess_count;j++) {
                    double nxt_dst = min_dsts[j + 1];
                    double d = min_dsts[j];
                    if (d >= nxt_dst) {
                        break;
                    }
                    int tmp_key_size = key_sizes[j + 1];
                    min_dsts[j] = nxt_dst;
                    min_dsts[j + 1] = d;
                    key_sizes[j + 1] = key_sizes[j];
                    key_sizes[j] = tmp_key_size;
                }
            }
        }
        std::string key = "";
        double max_score = 0;
        for (int i = 0; i < guess_count;i++) {
            //std::cout << "Bjorn" << std::endl;
            int key_size_guess = key_sizes[i];
            if (key_size_guess == 0) {
                continue;
            }
            std::vector<std::string> blocks{key_size_guess};
            for (size_t j = 0; j < l;j++) {
                blocks[j % key_size_guess].push_back(ct[j]);
            }
            std::string key_candidate = "";
            for (std::string block: blocks) {
                char c = frequency::detect_single_byte_xor(block);
                key_candidate.push_back(c);
            }
            std::string pt = repeating_XOR(ct, key_candidate, key_candidate.size());
            double score = frequency::score_string(pt.c_str(), pt.size());
            if (score > max_score) {
                max_score = score;
                key = key_candidate;
            }
        }
        return repeating_XOR(ct, key, key.size());
    }
}