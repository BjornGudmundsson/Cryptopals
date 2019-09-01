#ifndef BREAKS_HPP
#define BREAKS_HPP
#include <string>
#include "modes.hpp"
namespace BreakModes {
    //detect_block_size takes in a pointer to a ModeEncryptor and 
    //returns the block size that the ModeEncryptor uses.
    size_t detect_block_size(encryptionModes::ModeEncryptor *enc);
    
    //is_ECB takes in a mode encryption objects and says
    //true if it encrypts using ECB and false if it does not
    //encrypt using ECB.
    bool is_ECB(encryptionModes::ModeEncryptor *enc);

    std::string byte_at_a_time_ECB(encryptionModes::ModeEncryptor *enc, std::string flag);
    std::string byte_at_a_time_ECB_with_random_prefix(encryptionModes::ModeEncryptor *enc, std::string flag, size_t block_size);
    std::string CBC_bitflip_padding(encryptionModes::CBCEncryptor *enc, std::string user_data, size_t block_size);
    std::string CBC_bitflip_padding_attack(encryptionModes::CBCEncryptor *dec, size_t block_size, std::string ct);
    std::string get_random_string_for_CBC_padding();
    std::string CBC_padding_oracle_attack(std::string ct, std::string IV, encryptionModes::CBCEncryptor *enc, size_t block_size);
}
#endif