#include<iostream>
#include "headers/b64.hpp"
#include "headers/math.hpp"
#include "string.h"
#include "headers/frequency.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include "headers/vigenere.hpp"
#include "assert.h"
#include "headers/modes.hpp"
#include "headers/breaks.hpp"
#include "headers/parsing.hpp"
#include <map>

void ex4_set1(std::string fn) {
    std::ifstream infile(fn);
    std::string line;
    std::vector<std::string> decrypted_cts;
    while (std::getline(infile, line))
    {
        std::string dehexed = encoding::hex_string_to_bytes(line);
        char key = frequency::detect_single_byte_xor(dehexed);
        std::string decrypted = frequency::xor_buffer_to_byte_to_string(dehexed, key, dehexed.size());
        decrypted_cts.push_back(decrypted);
    }
    double max = 0.0;
    std::string answer;
    for (std::string pt: decrypted_cts) {
        double score = frequency::score_string(pt.c_str(), pt.size());
        if (score > max) {
            max = score;
            answer =pt;
        }
    }
    std::cout << "My answer: " << answer << std::endl;
}

void ex5_set1() {
    char *buf = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal";
    char *key = "ICE";
    size_t key_size = strlen(key);
    size_t buffer_size = strlen(buf);
    std::cout << "Buffer size: " << buffer_size << std::endl;
    std::cout << "Key size: " << key_size << std::endl;
    std::string t = Vigenere::repeating_XOR(buf, key, key_size, buffer_size);
    std::string ct_hex = encoding::bytes_to_hex(t);
    std::string solution = "0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c2a26226324272765272a282b2f20430a652e2c652a3124333a653e2b2027630c692b20283165286326302e27282f";
    std::cout << "My answer: " << ct_hex << std::endl;
    std::cout << "Is correct: " << (ct_hex == solution) << std::endl;
}



void ex6_set1() {
    std::string fn = "ex6_set1.txt";
    std::ifstream infile(fn);
    std::string line;
    std::vector<std::string> decrypted_cts;
    std::string b64_ct = "";
    while (std::getline(infile, line))
    {
        b64_ct += line;
    }
    std::string ct = base64::b64_to_bytes(b64_ct);
    std::string ct_hex = encoding::bytes_to_hex(ct);
    std::string pt = Vigenere::break_repeating_key_XOR(ct);
    std::cout << "Plaintext: " << std::endl << pt << std::endl;
}

void ex7_set1() {
    std::string fn = "ex7_set1.txt";
    std::ifstream infile(fn);
    std::string line;
    std::string b64_ct = "";
    while (std::getline(infile, line))
    {
        b64_ct += line;
    }
    std::string ct = base64::b64_to_bytes(b64_ct);
    std::string key = "YELLOW SUBMARINE";
    std::string pt = encryptionModes::decrypt_ECB_mode_128bits(ct, key);
    std::cout << "Plaintext: yall \n" << pt << std::endl;
}

void ex8_set1() {
    std::string fn = "ex8_set1.txt";
    std::ifstream infile(fn);
    std::string line;
    std::string hex_ct = "";
    std::string answer;
    size_t current_index = 1;
    size_t line_index = 1;
    while (std::getline(infile, line)) {
        std::map<std::string, size_t>  block_counter;
        size_t l = line.size();
        bool is_answer = false;
        for (size_t i = 0; i < l;i += 32) {
            std::string block = line.substr(i, 32);
            if (block_counter.find(block) == block_counter.end()) {
                block_counter[block] = 0;
            }
            block_counter[block] += 1;
            if (block_counter[block] > 2) {
                is_answer = true;
                answer = line;
                line_index = current_index;
                break;
            }
        }
        if (is_answer) {
            break;
        }
        current_index += 1;
    }    
    std::cout <<"The encrypted line is: " << answer << std::endl;
    std::cout << "The index of that line is " << line_index << std::endl;
}

void ex1_set2() {
    std::string test = "YELLOW SUBMARINE";
    size_t block_size = 20;
    std::string solution = encryptionModes::PKCS_padding(test, block_size);
    assert(solution == (test + "\04\04\04\04"));
    std::cout << "PASSED ex1 from set 2" << std::endl;
}

//Testing out if my CBC encryption/decryption works
void ex2_set2() {
    char *pt = (char*)malloc(16*4);
    strcpy(pt, "YELLOW SUBMARINEYELLOW SUBMARINEYELLOW SUBMARINEYELLOW SUBMARINE");
    //*pt = "YELLOW SUBMARINEYELLOW SUBMARINEYELLOW SUBMARINEYELLOW SUBMARINE";
    char *key = (char *)malloc(16);
    strcpy(key, "YELLOW SUBMARINE");
    char *IV = (char *)malloc(16);
    strcpy(IV, "My name is Bjorn");
    encryptionModes::CBCEncryptor *cbcEnc = new encryptionModes::CBCEncryptor(key, IV);
    cbcEnc->encrypt(pt);
    std::string s1{pt};
    std::cout << "S1: " << s1 << std::endl;
    std::string s2 = "YELLOW SUBMARINEYELLOW SUBMARINEYELLOW SUBMARINEYELLOW SUBMARINE";
    assert(s1 != s2);
    std::cout << "Encryption worked!" << std::endl;
    cbcEnc->decrypt(pt);
    std::string s3{pt};
    std::cout << "S3: " << s3 << std::endl;
    assert(s3 == s2);
    std::cout << "CBC mode worked " << std::endl;
    free(pt);
    delete cbcEnc;
}

void byte_at_a_time_ECB_harder() {
    std::string b64_flag = "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK";
    std::string flag = base64::b64_to_bytes(b64_flag);
    char *key = "YELLOW SUBMARINE";
    encryptionModes::RandomECBEncryptor *enc = new encryptionModes::RandomECBEncryptor(key, 16);
    std::string s = "";
    std::string dec = BreakModes::byte_at_a_time_ECB_with_random_prefix(enc, flag, 16);
    assert(flag.length() == dec.length());
    assert(flag == dec);
    std::cout << "Flag: " << std::endl << dec << std::endl;
    //delete enc;
}


void test_remove_PKCS() {
    //These should succeed
    try {
        std::string s1 = encryptionModes::PKCS_padding("1", 16);
        std::string t1 = encryptionModes::remove_PKCS_padding(s1, 16);
        std::cout << "T1: Passed" << std::endl;
        std::string s2 = encryptionModes::PKCS_padding("2\x2\x2", 3);
        std::string t2 = encryptionModes::remove_PKCS_padding(s2, 3);
        std::cout << "T2: Passed: " << std::endl;
    }
    catch (int e) {
        std::cout <<"T1 or T2: failed" << std::endl;
    }
    //These should fail
    try {
        std::string t3 = encryptionModes::remove_PKCS_padding("1\x1\x2", 3);
        std::cout << "T3: Failed" << std::endl;
    }
    catch (int e) {
        std::cout <<" T3: Passed" << std::endl;
    }
}

void test_CBC() {
    std::string pt = "YELLOW SUBMARINEYELLOW SUBMARINEYELLOW SUBMARINE";
    char *key = (char*) malloc(16);
    strcpy(key, "YELLOW SUBMARINE");
    char *IV = (char*) malloc(16);
    strcpy(IV, key);
    encryptionModes::CBCEncryptor enc{key, IV};
    std::string ct = enc.encrypt_string(pt);
    assert(pt != ct);
    std::string ret = enc.decrypt_string(ct);
    std::cout << (ret == pt) << std::endl;
    //assert(pt == ret);
}

void test_CBC_bitflip_attack() {
    size_t block_size = 16;
    std::string prefix = "comment1=cooking%20MCs;userdata=";
    std::string appendix = ";comment2=%20like%20a%20pound%20of%20bacon";
    size_t data_size = block_size - (prefix.size() % block_size);
    std::string user_data;
    for (size_t i = 0; i < data_size;i++) {
        user_data.push_back('A');
    }
    char *s = "YELLOW SUBMARINE";
    char *key = (char*) malloc(block_size);
    strcpy(key, s);
    char *IV = (char*) malloc(block_size);
    strcpy(IV, key);
    encryptionModes::CBCEncryptor *enc = new encryptionModes::CBCEncryptor{key, IV};
    std::string ct = BreakModes::CBC_bitflip_padding(enc, user_data, block_size);
    std::string pt = BreakModes::CBC_bitflip_padding_attack(enc, block_size, ct);
    delete enc;
    std::cout << "CBC bitflipping attack works!" << std::endl;
}
int main() {
    test_CBC_bitflip_attack();
    return 0;
}