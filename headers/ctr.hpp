#ifndef CTR_HPP
#define CTR_HPP
#include <iostream>
namespace ctr {
    std::string little_endian_64bit(unsigned long long d);

    class CTR_Mode {
        private:
            char *key;
            unsigned long long nonce;
            size_t block_size;
        public:
            CTR_Mode(char *key, unsigned long long nonce, size_t block_size);
            ~CTR_Mode();
            std::string encrypt_string(std::string pt);
            std::string decrypt_string(std::string ct);
    };
}
#endif