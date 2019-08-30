#ifndef MODES_HPP
#define MODES_HPP
#include <string>
#include "../headers/math.hpp"
namespace encryptionModes {
    std::string decrypt_ECB_mode_128bits(std::string ct, std::string key);
    std::string PKCS_padding(std::string s, size_t block_size);
    std::string remove_PKCS_padding(std::string s, size_t block_size);
    void encrypt_ECB_mode_128bits(char *pt, char *key);
    void decrypt_CBC_mode_128bits(char *ct, char *key, char *IV);
    void encrypt_CBC_mode_128bits(char *pt, char *key, char *IV);

    //Classes
    class ModeEncryptor {
        private:
        public:
            virtual void encrypt(char *pt) = 0;
            virtual void decrypt(char *pt) = 0;
            virtual std::string encrypt_string(std::string pt) = 0;
            virtual std::string decrypt_string(std::string ct) = 0;
    };

    class CBCEncryptor: public ModeEncryptor {
        private:
            char *key;
            char *IV;
        public:
            void encrypt(char *pt);
            void decrypt(char *pt);
            std::string encrypt_string(std::string pt);
            std::string decrypt_string(std::string ct);
            CBCEncryptor(char *key, char*IV);
            ~CBCEncryptor();
    };

    class RandomEncryptor: public ModeEncryptor {
        public:
            RandomEncryptor();
            ~RandomEncryptor();
            void encrypt(char *pt);
            void decrypt(char *ct);
            std::string encrypt_string(std::string pt);
            std::string decrypt_string(std::string ct);
    };

    class ECBEncryptor: public ModeEncryptor {
        public:
            ECBEncryptor(char *key);
            ~ECBEncryptor();
            void encrypt(char *pt);
            void decrypt(char *ct);
            std::string encrypt_string(std::string pt);
            std::string decrypt_string(std::string ct);
        private:
            char *key;
            size_t block_size;
    };

    class RandomECBEncryptor: public ModeEncryptor {
        public:
            RandomECBEncryptor(char *key, size_t block_size);
            ~RandomECBEncryptor();
            void encrypt(char *pt);
            void decrypt(char *ct);
            std::string encrypt_string(std::string pt);
            std::string decrypt_string(std::string ct);
        private:
            char *key;
            size_t block_size;
            encoding::RandomGenerator rc;
            std::string prefix;
    };

    bool CBC_admin_validator(CBCEncryptor *enc, std::string ct);
}
#endif