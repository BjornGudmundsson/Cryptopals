#include <string>
#include "../headers/modes.hpp"
#include "../headers/parsing.hpp"
#include "../headers/b64.hpp"
#include "assert.h"
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
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

    std::string get_block_in_map(std::vector<std::map<std::string, char>> block_vec, std::string ct, size_t block_size) {
        for (std::map<std::string, char> block_map: block_vec) {
            size_t l = ct.size();
            for (size_t pos = 0; pos < l;pos += block_size) {
                std::string block = ct.substr(pos, block_size);
                if (block_map.find(block) != block_map.end()) {
                    return block;
                }
            }
        }
        return "";
    }

    size_t get_size_of_prefix(encryptionModes::ModeEncryptor *enc, std::string flag) {
        std::string block = enc->encrypt_string(flag);
        size_t default_block_size = block.size();
        char A = 'A';
        std::string padding = "";
        int count = 0;
        while (block.size() == default_block_size) {
            padding.insert(0, 1, A);
            block = enc->encrypt_string(padding + flag);
            count++;
        }
        return count - 1; 
    }

    std::string get_base_padding(size_t prefix_size) {
        std::string padding = "";
        for (size_t i = 0; i < prefix_size;i++)  {
            padding.push_back('A');
        }
        return padding;
    }

    std::map<std::string, char> get_block_char_with_prefix(std::string known_part, size_t block_size, encryptionModes::ModeEncryptor *enc) {
        size_t l = known_part.size();
        std::string last_part_of_block;
        std::map<std::string, char> block_to_char;
        if (l >= block_size) {
            last_part_of_block = known_part.substr(0, block_size - 1);
        }
        else {
            last_part_of_block = encryptionModes::PKCS_padding(known_part, block_size - 1);
        }
        size_t prefix_size = get_size_of_prefix(enc, "");
        std::string base_padding = get_base_padding(prefix_size);
        for (int i = -128; i <= 127;i++) {
            char c = (char) i;
            std::string next_block = last_part_of_block;
            next_block.insert(0, 1, c);
            std::string ct = enc->encrypt_string(base_padding + next_block);
            std::string ct_block = ct.substr(block_size, block_size);
            block_to_char[ct_block] = c;
        }
        return block_to_char;
    }

    char get_next_char(std::string known_part, std::string ct, encryptionModes::ModeEncryptor *enc, size_t block_size) {
        std::map<std::string, char> block_to_char = get_block_char_with_prefix(known_part, block_size, enc);
        size_t l = ct.size();
        for (size_t pos = 0; pos < l;pos += block_size) {
            std::string block = ct.substr(pos, block_size);
            if (block_to_char.find(block) != block_to_char.end()) {
                return block_to_char[block];
            }
        }
        return '\x0';
    }

    std::string byte_at_a_time_ECB_with_random_prefix(encryptionModes::ModeEncryptor *enc, std::string flag, size_t block_size) { 
        size_t prefix_size = get_size_of_prefix(enc, flag);
        std::string base_padding = get_base_padding(prefix_size);
        //Now we know it is a multiple of the block size - no added padding.
        //Storing what bytes we already know.
        size_t l = flag.size();
        std::string known_part = "";
        for (size_t i = 0; i < l;i++) {
            base_padding.push_back('A');
            std::string ct = enc->encrypt_string(base_padding + flag);
            char next_byte = get_next_char(known_part, ct, enc, block_size);
            known_part.insert(0, 1, next_byte);
        }
        return known_part;
    }

    std::string CBC_bitflip_padding(encryptionModes::CBCEncryptor *enc, std::string user_data, size_t block_size) {
        std::string prefix = "comment1=cooking%20MCs;userdata=";
        std::string appendix = ";comment2=%20like%20a%20pound%20of%20bacon";
        std::vector<std::string> split_by_semi_colon = parsing::splitString(user_data, ';');
        std::string join;
        for (std::string s: split_by_semi_colon) {
            join += s;
        }
        std::vector<std::string> split_by_equal_sign = parsing::splitString(join, '=');
        std::string ud;
        for (std::string s: split_by_equal_sign) {
            ud += s;
        }
        std::string pt = prefix + ud + appendix;
        std::string ct = enc->encrypt_string(pt);
        return ct;
    }

    std::string CBC_bitflip_padding_attack(encryptionModes::CBCEncryptor *dec, size_t block_size, std::string ct) {
        std::string prefix = "comment1=cooking%20MCs;userdata=";
        std::string appendix = ";comment2=%20like%20a%20pound%20of%20bacon";
        size_t start_index = ct.size() - appendix.size() - block_size;
        std::string attack = encryptionModes::PKCS_padding(";admin=true;", block_size);
        std::string pt_attack = appendix.substr(0, block_size);
        for (size_t i = 0; i < block_size;i++) {
            ct[start_index + i] ^= (attack[i] ^ pt_attack[i]);
        }
        return dec->decrypt_string(ct);
    }

    bool decryption_is_valid_padding(std::string ct, encryptionModes::CBCEncryptor *enc, size_t block_size, std::string IV) {
        std::string pt = enc->decrypt_string_with_custom_IV(ct, IV);
        bool valid;
        try {
            std::string t = encryptionModes::remove_PKCS_padding(pt, block_size);
            valid = true;
        }
        catch(int e) {
            valid = false;
        }
        return valid;
    }

    std::string get_partial_padding(char c, size_t count) {
        std::string ret;
        for (size_t i = 0; i < count;i++) {
            ret.push_back(c);
        }
        return ret;
    }

    std::string get_random_string_for_CBC_padding() {
        std::string s1 = "MDAwMDAwTm93IHRoYXQgdGhlIHBhcnR5IGlzIGp1bXBpbmc=";
        std::string s2 = "MDAwMDAxV2l0aCB0aGUgYmFzcyBraWNrZWQgaW4gYW5kIHRoZSBWZWdhJ3MgYXJlIHB1bXBpbic=";
        std::string s3 = "MDAwMDAyUXVpY2sgdG8gdGhlIHBvaW50LCB0byB0aGUgcG9pbnQsIG5vIGZha2luZw==";
        std::string s4 = "MDAwMDAzQ29va2luZyBNQydzIGxpa2UgYSBwb3VuZCBvZiBiYWNvbg==";
        std::string s5 = "MDAwMDA0QnVybmluZyAnZW0sIGlmIHlvdSBhaW4ndCBxdWljayBhbmQgbmltYmxl";
        std::string s6 = "MDAwMDA1SSBnbyBjcmF6eSB3aGVuIEkgaGVhciBhIGN5bWJhbA==";
        std::string s7 = "MDAwMDA2QW5kIGEgaGlnaCBoYXQgd2l0aCBhIHNvdXBlZCB1cCB0ZW1wbw==";
        std::string s8 = "MDAwMDA3SSdtIG9uIGEgcm9sbCwgaXQncyB0aW1lIHRvIGdvIHNvbG8=";
        std::string s9 = "MDAwMDA4b2xsaW4nIGluIG15IGZpdmUgcG9pbnQgb2g=";
        std::string s10 = "MDAwMDA5aXRoIG15IHJhZy10b3AgZG93biBzbyBteSBoYWlyIGNhbiBibG93";
        std::vector<std::string> vec{s1, s2, s3, s4, s5, s6, s7, s8, s9, s10};
        encoding::RandomGenerator rc{};
        int n = rc.generate_random_num(9, 0);
        return base64::b64_to_bytes(vec[n]);
    }

    std::string xor_two_string(std::string s1, std::string s2) {
        size_t l = s1.size();
        if (l != s2.size()) {
            return "";
        }
        std::string ret;
        for (size_t i = 0; i < l;i++) {
            ret.insert(0, 1, s1[i] ^ s2[i]);
        }
        return ret;
    }

    std::string CBC_padding_oracle_attack(std::string ct, std::string IV, encryptionModes::CBCEncryptor *enc, size_t block_size) {
        size_t l = ct.size();
        size_t block_count = l / block_size;
        //This is a buffer to store the found bytes
        std::string pt;
        std::string attack_block{IV};
        //Finding the bytes of the CT one byte at a time
        for (size_t i = 0; i < block_count;i++) { 
            //iterate over all bytes
            std::string victim_block{ct.substr(i * block_size, block_size)};
            std::string known_bytes = "";
            std::string appendix;
            size_t padding = 0;
            std::string all_ones = get_partial_padding('\xff', block_size);
            char all_one = '\xff';
            std::string s = xor_two_string(all_ones, attack_block);
            while (!decryption_is_valid_padding(victim_block, enc, block_size, s) && padding != block_size) 
            {
                all_ones[block_size - 1 - padding] = '\x0';
                padding++;
                s = xor_two_string(all_ones, attack_block);
            }
            if (padding != block_size) {
                for (size_t p = 0; p < padding;p++) {
                    known_bytes.push_back((char) padding);
                }
            }
            for (size_t b = 0; b <= block_size;b++) {
                size_t count_known_bytes = known_bytes.size();
                char padding = (char) (count_known_bytes + 1);
                if (count_known_bytes != 0) {
                    for (size_t k = 0; k < count_known_bytes;k++) {
                        appendix.push_back(padding);
                    }
                }
                if (count_known_bytes == block_size) {
                    break;
                }
                for (int j = 127; j >= -128;j--) {
                    char c = (char) j;
                    std::string temp_attack_block{attack_block};
                    for (size_t pos = 0; pos < count_known_bytes;pos++) {
                        temp_attack_block[block_size - 1 - pos] ^= (known_bytes[count_known_bytes - 1 - pos] ^ padding);
                    }
                    temp_attack_block[block_size - 1 - count_known_bytes] ^= (c ^ padding);
                    //std::cout << "Decrypting: " << j << std::endl;
                    bool has_valid_padding = decryption_is_valid_padding(temp_attack_block + victim_block, enc, block_size, temp_attack_block);
                    if (has_valid_padding) {
                        known_bytes.insert(0, 1, c);
                        break;
                    }
                }
            }
            if (known_bytes.size() == block_size) {
                pt += known_bytes;   
            }
            attack_block = victim_block;
        }
        //Handling the last block seperately
        return pt;
    }
}