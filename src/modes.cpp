#include "cryptopp/aes.h"
#include <iostream>
#include "../headers/modes.hpp"
#include "../headers/math.hpp"
using namespace CryptoPP;

namespace encryptionModes
{
    std::string PKCS_padding(std::string s, size_t block_size) {
        size_t l = s.size();
        size_t m = l % block_size;
        if (m == 0) {
            return s;
        }
        char pad_char = (char)(block_size - m);
        for (char i = 0; i < pad_char;i++) {
            s.push_back(pad_char);
        }
        return s;
    }

    std::string remove_PKCS_padding(std::string s, size_t block_size) {
        size_t l = s.size();
        if (l == 0) {
            return s;
        }
        int invalid = -1;
        if (l % block_size != 0) {
            throw invalid;
        }
        char last_byte = s[l - 1];
        if (last_byte >= block_size) {
            return s;
        }
        if (last_byte > ((char) l)) {
            //Throw -1 in case there i
            throw invalid;
        }
        for (char i = 1; i < last_byte && i > 0;i++) {
            char nxt_byte = s[l - 1 - i];
            if (nxt_byte != last_byte) {
                throw invalid;
            }
        }
        std::string sub = s.substr(0, l - last_byte);
        if (sub.size() % block_size != (block_size - last_byte)) {
            throw invalid;
        }
        return sub;
    }

    std::string decrypt_ECB_mode_128bits(std::string ct, std::string key) {
        size_t  block_size = 16;
        size_t l = ct.size();
        size_t m = l % block_size;
        byte *key_bytes = (byte*)key.c_str();
        AES::Decryption aesDecryption(key_bytes, AES::DEFAULT_KEYLENGTH);
        if (m != 0) {
            size_t pad = 16 - m;
            for (size_t i = 0; i < pad;i++) {
                ct.push_back('\0');
            }
        }
        std::string ret = "";
        l = ct.size();
        for (size_t i = 0; i < l;i += block_size) {
            std::string sub = ct.substr(i, block_size);
            byte *block_byte = (byte*)sub.c_str();
            aesDecryption.ProcessBlock(block_byte);
            std::string t{(char*)block_byte};
            ret += t;
        }
        return ret;
    }

    std::string encrypt_ECB_mode_128bits(std::string pt, std::string key) {
        std::string padded = PKCS_padding(pt, key.size());
        size_t  block_size = key.size();
        size_t l = padded.size();
        size_t m = l % block_size;
        byte *key_bytes = (byte*)key.c_str();
        AES::Encryption aesEncryption(key_bytes, AES::DEFAULT_KEYLENGTH);
        if (m != 0) {
            size_t pad = 16 - m;
            for (size_t i = 0; i < pad;i++) {
                pt.push_back('\0');
            }
        }
        std::string ret = "";
        l = pt.size();
        for (size_t i = 0; i < l;i += block_size) {
            std::string sub = padded.substr(i, block_size);
            byte *block_byte = (byte*)sub.c_str();
            aesEncryption.ProcessBlock(block_byte);
            std::string t;
            for (size_t i = 0; i < block_size;i++) {
                t.push_back(block_byte[i]);
            }
            ret += t;
        }
        return ret;
    }

    void encrypt_ECB_mode_128bits(char *pt, char *key) {
        byte *pt_bytes = (byte*)pt;
        byte *key_bytes = (byte*)key;
        AES::Encryption aesEncryption{key_bytes, AES::DEFAULT_KEYLENGTH};
        aesEncryption.ProcessBlock(pt_bytes);
    }

    void encrypt_ECB_mode_128bits(char *pt, char* key, size_t key_size, size_t pt_size) {
        size_t l = pt_size / key_size;
        char *current_block = pt;
        for (size_t i = 0; i < l;i += key_size) {
            encrypt_ECB_mode_128bits(current_block, key);
            current_block += key_size;
        }
    }

    void decrypt_ECB_mode_128bits(char *pt, char* key, size_t key_size, size_t pt_size) {
        size_t l = pt_size / key_size;
        char *current_block = pt;
        for (size_t i = 0; i < l;i += key_size) {
            decrypt_ECB_mode_128bits(current_block, key);
            current_block += key_size;
        }
    }

    void decrypt_ECB_mode_128bits(char *ct, char *key) {
        byte *ct_bytes = (byte*)ct;
        byte *key_bytes = (byte*)key;
        AES::Decryption aesDecryption{key_bytes, AES::DEFAULT_KEYLENGTH};
        aesDecryption.ProcessBlock(ct_bytes);
    }

    //encrypt_CBC_mode_128bits takes a buffer and encrypts it using 
    //AES128 bits with the corresponding key and IV. It assumes that the 
    //plaintext size is a multiple of the blocksize.
    void encrypt_CBC_mode_128bits(char *pt, char *key, char *IV) {
        //The size of each block;
        size_t l = strlen(key);
        //Total size of the plaintext
        size_t pt_size = strlen(pt);
        char *current_block = pt;
        char *current_IV = IV;
        size_t iterations = pt_size / l;
        for (size_t i = 0; i < iterations;i++) {
            //std::cout << "in loop " << strlen(key) <<  std::endl;
            encoding::xor_eq_len_buffers(current_block, current_IV, current_block, l);
            encrypt_ECB_mode_128bits(current_block, key);
            current_IV = current_block;
            current_block += l;
        }
    }

    void decrypt_CBC_mode_128bits(char *ct, char *key, char *IV) {
        //The size of each block;
        size_t l = strlen(key);
        //Total size of the plaintext
        size_t ct_size = strlen(ct);
        size_t iterations = ct_size / l;
        for (size_t i = iterations - 1; i > 0;i--) {
            char *curr_block = ct + i * l;
            char *curr_IV = ct + (i - 1) * l;
            decrypt_ECB_mode_128bits(curr_block, key);
            encoding::xor_eq_len_buffers(curr_block, curr_IV, curr_block, l);
        }
        decrypt_ECB_mode_128bits(ct, key);
        encoding::xor_eq_len_buffers(ct, IV, ct, l);
    }

    CBCEncryptor::CBCEncryptor(char *key, char *IV) {
        this->IV = IV;
        this->key = key;
    }

    CBCEncryptor::~CBCEncryptor() {
        free(this->IV);
        free(this->IV);
    }

    void CBCEncryptor::encrypt(char *pt) {
        encrypt_CBC_mode_128bits(pt, this->key, this->IV);
    }
    void CBCEncryptor::decrypt(char *ct) {
        decrypt_CBC_mode_128bits(ct, this->key, this->IV);
    }

    std::string CBCEncryptor::encrypt_string(std::string pt ) {
        size_t l = pt.size();
        char *temp = (char*)malloc(l);
        for (size_t i = 0; i < l;i++) {
            temp[i] = pt[i];
        }
        std::string ret;
        encrypt_CBC_mode_128bits(temp, this->key, this->IV);
        for (size_t i = 0; i < l;i++) {
            ret.push_back(temp[i]);
        }
        return ret;
    }

    std::string CBCEncryptor::decrypt_string(std::string ct) {
        size_t l = ct.size();
        char *temp = (char*) malloc(l);
        for (size_t i = 0; i < l;i++) {
            temp[i] = ct[i];
        }
        decrypt_CBC_mode_128bits(temp, this->key, this->IV);
        std::string ret;
        for (size_t i = 0; i < l;i++) {
            ret.push_back(temp[i]);
        }
        return ret;
    }

    RandomEncryptor::RandomEncryptor() {
        //Empty
    }

    RandomEncryptor::~RandomEncryptor() {
        //Empty
    }

    void RandomEncryptor::encrypt(char *pt) {
        //Wasn't necessary to enable this for the exercise
    }

    void RandomEncryptor::decrypt(char *ct) {
        //Wasn't necessary to enable this for the exercise
    }

    std::string RandomEncryptor::encrypt_string(std::string pt) {
        encoding::RandomGenerator r;
        std::string padded = r.random_pad_string(pt);
        std::cout << "Padded length: " << padded.size() << std::endl;
        std::string fixed = PKCS_padding(padded, 16);
        size_t l = fixed.size();
        char *t = (char*)malloc(l);
        strcpy(t, fixed.c_str());
        char *key = r.random_key(16);
        char *IV = r.random_IV(16);
        int bit = r.generate_random_num(1, 0);
        if (bit) {
            encrypt_ECB_mode_128bits(t, key, 16, l);
            std::string s{t};
            return s;
        }
        std::string s2{IV};
        encrypt_CBC_mode_128bits(t, key, IV);
        std::string s{t};
        return s;
    }

    std::string RandomEncryptor::decrypt_string(std::string ct) {
        return "";
    }

    ECBEncryptor::ECBEncryptor(char *key) {
        char *k = (char*)malloc(strlen(key));
        strcpy(k, key);
        this->key = k;
        this->block_size  = 16;
    }

    ECBEncryptor::~ECBEncryptor() {
        free(this->key);
    }

    void ECBEncryptor::encrypt(char *pt) {
        encrypt_ECB_mode_128bits(pt, this->key, this->block_size, strlen(pt));
    }

    void ECBEncryptor::decrypt(char *ct) {
        decrypt_ECB_mode_128bits(ct, key, this->block_size, strlen(ct));
    }

    std::string ECBEncryptor::encrypt_string(std::string pt) {
        std::string sk{this->key};
        return encrypt_ECB_mode_128bits(pt, sk);
    }

    std::string ECBEncryptor::decrypt_string(std::string ct) {
        return decrypt_ECB_mode_128bits(ct, this->key);
    }

    RandomECBEncryptor::RandomECBEncryptor(char *key, size_t block_size) {
        this->key = key;
        this->block_size = block_size;
        this->rc = encoding::RandomGenerator{};
        this->prefix = rc.random_prefix_string("", this->block_size);
    }

    RandomECBEncryptor::~RandomECBEncryptor() {
        free(this->key);
    }

    void RandomECBEncryptor::encrypt(char *pt) {
        //Does nothing
    }

    void RandomECBEncryptor::decrypt(char *ct) {
        //TODO:
    }

    std::string RandomECBEncryptor::encrypt_string(std::string pt) {
        std::string prefixed = this->prefix + pt;
        std::string padded_prefix = PKCS_padding(prefixed, this->block_size);
        //std::cout << padded_prefix.substr(10, 20) << std::endl;
        std::string ct = encrypt_ECB_mode_128bits(padded_prefix, this->key);
        return ct;
    }

    std::string RandomECBEncryptor::decrypt_string(std::string ct) {
        return decrypt_ECB_mode_128bits(ct, this->key);
    }
} // namespace encryptionModes 