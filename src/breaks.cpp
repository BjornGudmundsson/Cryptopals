#include <string>
#include "../headers/modes.hpp"
#include <iostream>
#include <map>
namespace BreakModes {
    size_t detect_block_size(encryptionModes::ModeEncryptor *enc) {
        std::string A = "A";
        //It works under the assumption that the mode encryptor will pad the string 
        //to a multiple of the block size.
        std::string ct = enc->encrypt_string(A);
        size_t current_size = ct.size();
        for (;true;) {
            A.push_back('A');
            std::string c = enc->encrypt_string(A);
            size_t l = c.size();
            if (current_size < l) {
                return l - current_size;
            }
        }
        return ct.size();
    }

    //is_ECB takes in a mode encryption objects and says
    //true if it encrypts using ECB and false if it does not
    //encrypt using ECB.
    bool is_ECB(encryptionModes::ModeEncryptor *enc) {
        std::string test_string;
        size_t block_size = detect_block_size(enc);
        //Create a string such that it is guaranteed that atleast 3 blocks will be adjacent to each other.
        for (size_t i = 0; i < 5 * block_size;i++) {
            test_string.push_back('A');
        }
        std::string ct = enc->encrypt_string(test_string);
        size_t l = ct.size();
        size_t count = 0;
        std::string current_block;
        for (size_t i = 0; i < l;i += block_size) {
            std::string block = ct.substr(i, block_size);
            //std::cout << "Block: " << block << std::endl;
            if (current_block != block) {
                count = 1;
                current_block = block;
                continue;
            }
            count++;
            if (count >= 3) {
                return true;
            }
        }
        return false;
    }

    std::map<std::string, char> block_to_chars(size_t block_size, encryptionModes::ModeEncryptor *enc) {
        std::map<std::string, char> block_map;
        //I just wanted the string to be of length 1
        for (int i = -128; i <= 127;i++) {
            char c = (char) i;
            std::string s;
            s.push_back(c);
            std::string padded = encryptionModes::PKCS_padding(s, block_size);
            std::string ct = enc->encrypt_string(padded);
            block_map[ct] = c;
        }
        return block_map;
    }

    std::string find_most_occurring_ct_block(std::string ct, size_t block_size) {
        std::string most_common_block;
        size_t max_count = 0;
        size_t l = ct.size();
        std::map<std::string, int> count_map;
        for (size_t pos = 0; pos < l;pos += block_size) {
            std::string block = ct.substr(pos, block_size);
            if (count_map.find(block) == count_map.end()) {
                count_map[block] = 0;
            }
            count_map[block] = count_map[block] + 1;
            int count = count_map[block];
            if (count > max_count) {
                std::cout << "new block" << std::endl;
                max_count = count;
                most_common_block = block;
            }
        }
        return most_common_block;
    }

    std::string shift_string_and_put(std::string s, char c) {
        char temp = c;
        std::string s2 = s;
        size_t l = s2.size();
        for (size_t i = 0; i < l;i++) {
            char t = s2[i];
            s2[i] = temp;
            temp = t;
        }
        return s2;
    }

    std::map<std::string, char> block_to_chars_with_random_prefix(size_t block_size, encryptionModes::ModeEncryptor *enc, std::string known_part) {
        std::map<std::string, char> block_map;
        for (int i = -128; i <= 127;i++) {
            char c = (char) i;
            std::string ct;
            size_t known_size = known_part.size();
            std::string most_common_block;
            if (known_size == block_size) {
                std::string shifted = shift_string_and_put(known_part, c);
                std::string shiftedx5 = shifted + shifted + shifted + shifted +shifted;
                ct = enc->encrypt_string(shiftedx5);
                most_common_block = find_most_occurring_ct_block(ct, block_size);
            }
            else {
                std::string s = known_part;
                s.insert(0, 1, c);
                std::string padded = encryptionModes::PKCS_padding(s, block_size);
                //I use 5 times the block to guarantee that there will be at least 2 blocks continous
                std::string paddedx5 = padded + padded + padded + padded + padded + padded + padded;
                ct = enc->encrypt_string(paddedx5);
                most_common_block = find_most_occurring_ct_block(ct, block_size);
                //std::cout << "Most common: " << int(most_common_block[most_common_block.size() - 1]) << std::endl;
            }
            block_map[most_common_block] = c;
        }
        return block_map;
    }

    std::string byte_at_a_time_ECB(encryptionModes::ModeEncryptor *enc, std::string flag) {
        size_t block_size = detect_block_size(enc);
        bool is_ECB_encryptor = is_ECB(enc);
        if (!is_ECB_encryptor) {
            //Don't feel like making an exception for this
            return "";
        }
        std::map<std::string, char> block_map = block_to_chars(block_size, enc);
        std::string tmp(flag);
        size_t l = tmp.size();
        std::string pt;
        while (l != 0)
        {
            size_t m = (block_size - (l % block_size) + 1) % block_size;
            std::string padding = "";
            for (size_t i = 0; i < m;i++) {
                padding.insert(0, 1,'B');
            }
            std::string ct = enc->encrypt_string(padding + tmp);
            size_t cl = ct.size();
            std::string last_block = ct.substr(cl - block_size, block_size);
            if (block_map.find(last_block) == block_map.end()) {
                //std::cout << block_map.size() << std::endl;
                break;
            }
            char byte = block_map[last_block];
            pt.insert(0, 1, byte);
            l -= 1;
            tmp = tmp.substr(0, l);
        }
        return pt;
    }

    std::string get_block_in_map(std::map<std::string, char> block_map, std::string ct, size_t block_size) {
        size_t l = ct.size();
        for (size_t pos = 0; pos < l;pos += block_size) {
            std::string block = ct.substr(pos, block_size);
            if (block_map.find(block) != block_map.end()) {
                return block;
            }
        }
        return "";
    }

    std::string byte_at_a_time_ECB_with_random_prefix(encryptionModes::ModeEncryptor *enc, std::string flag, size_t block_size) { 
        size_t l = flag.size();
        std::string known_part = "";
        for (size_t i = 0; i < l;i++) {
            std::string ct;
            std::map<std::string, char> block_map = block_to_chars_with_random_prefix(block_size, enc, known_part);
            std::string block;
            while (!block.size() != 0) {
                ct = enc->encrypt_string(flag);
                block = get_block_in_map(block_map, ct, block_size);
                //No need for anymore controlled attacker input than the flag
            }
            std::cout << "Exited " << std::endl;
            char c = block_map[block];
            known_part.insert(0, 1, c);
        }
        return known_part;
    }
}