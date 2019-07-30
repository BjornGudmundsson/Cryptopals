#ifndef B64_HPP
#define B64_HPP
#include <iostream>
namespace base64 {
    std::string hex_to_b64(std::string hex);
    std::string b64_to_bytes(std::string b64);
}
#endif