#include "bcrypt.h"
#include <iostream>
#include <cstdlib>
#include <argon2.h>
#include <string>

void doubleHash(std::string key){
    constexpr uint32_t t_cost = 4;
    constexpr uint32_t m_cost = 65536; //64 Mib
    constexpr uint32_t parallelism = 2;
    std::string salt = "B2UVP3o/HEIIZT8ZAz95CT8/Pz90GzA/Pzk/P18/cj8NCg==";
    char hash_output[32];
    constexpr uint32_t HASHLEN = 32;

    int result = argon2id_hash_raw(t_cost, m_cost, parallelism, 
                                  key.c_str(), key.length(), 
                                  salt.c_str(), salt.length(), 
                                  hash_output, HASHLEN);
    char hex_hash[HASHLEN * 2 + 1];
    for (int i = 0; i < HASHLEN; ++i) {
        sprintf(hex_hash + (i * 2), "%02x", (unsigned char)hash_output[i]);
    }
    std::string first_hash(hex_hash);
    
    std::cout << "First hash: "+ first_hash <<std::endl;
    std::cout << "Second hash: "+ bcrypt::generateHash(first_hash, 12) <<std::endl;
}

int main(){
    std::string DAEMON_KEY;
    std::cout << "Enter your DAEMON_KEY: ";
    std::getline(std::cin, DAEMON_KEY);
    
    std::cout << "---------------------------" << std::endl;
    std::cout <<"You entered: \""+ DAEMON_KEY+"\"" << std::endl;
    doubleHash(DAEMON_KEY);
    std::cout << "---------------------------" << std::endl;
    std::cout << "Press enter to continue";
    
    std::string null;
    std::getline(std::cin, null);
}
