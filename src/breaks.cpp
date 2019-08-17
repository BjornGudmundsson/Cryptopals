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
        std::cout << block_map.size() << std::endl;
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
                continue;
            }
            char byte = block_map[last_block];
            pt.insert(0, 1, byte);
            l -= 1;
            tmp = tmp.substr(0, l);
        }
        return pt;
    }
}