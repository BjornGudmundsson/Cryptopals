#include "../headers/ctr.hpp"
#include "../headers/modes.hpp"
#include "../headers/math.hpp"
#include <string>
#include <iostream>

namespace ctr
{
    std::string little_endian_64bit(unsigned long long x) {
        //Note: I wrote this for my processor which had little endian architecture.
        //On a different machine this would have to be rewritten.
        char *t = (char*)&x;
        int size = sizeof(unsigned long long);
        std::string ret;
        for (int i = 0; i < size;i++) {
            ret.push_back(t[i]);
        }
        return ret; 
    }

    CTR_Mode::CTR_Mode(char *key, unsigned long long nonce, size_t block_size) {
        this->key = key;
        this-> nonce;
        this->block_size = block_size;
    }
    CTR_Mode::~CTR_Mode() {
        free(key);
    }

    std::string CTR_Mode::encrypt_string(std::string pt) {
        size_t l = pt.size();
        unsigned long long  block_count = ((unsigned long long) l) / ((unsigned long long) this->block_size);
        size_t extra_chars = l % this->block_size;
        std::string key;
        std::string ct;
        std::string little_endian_nonce = little_endian_64bit(this->nonce);
        for (size_t i = 0; i < this->block_size;i++) {
            key.push_back(this->key[i]);
        }
        unsigned long long current_block_number = 0;
        for (; current_block_number < block_count;current_block_number++) {
            std::string little_endian_block_count = little_endian_64bit(current_block_number);
            std::string s = little_endian_nonce + little_endian_block_count;
            std::string block_key = encryptionModes::encrypt_ECB_mode_128bits(little_endian_nonce + little_endian_block_count, key);
            for (size_t i = 0; i < this->block_size;i++) {
                ct.push_back(pt[(current_block_number * this->block_size) + (unsigned long long) i] ^ block_key[i]);
            }
        }
        if (extra_chars != 0) {
            std::string little_endian_block_count = little_endian_64bit(current_block_number);
            std::string block_key = encryptionModes::encrypt_ECB_mode_128bits(little_endian_nonce + little_endian_block_count, key);
            for (size_t i = 0; i < extra_chars;i++) {
                ct.push_back(pt[(current_block_number * this->block_size) + (unsigned long long) i] ^ block_key[i]);
            }
        }
        return ct;
    }

    std::string CTR_Mode::decrypt_string(std::string pt) {
        return this->encrypt_string(pt);
    }
} // namespace ctr