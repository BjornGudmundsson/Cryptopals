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
}
#endif