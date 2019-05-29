#pragma once
#include <fc/time.hpp>
#include <fc/crypto/sha256.hpp>
#include <fc/crypto/public_key.hpp>
#include <fc/crypto/private_key.hpp>
#include <fc/io/json.hpp>
#include <vector>
#include <string>
class Transaction{

public:
    fc::time_point_sec time_point;
    std::vector<char> data;
    std::vector<char> attach;
    fc::crypto::signature signature;
    fc::crypto::public_key pub_key;
    
    fc::sha256 digest() const;
    bool validate() const;
};

FC_REFLECT(Transaction, (time_point)(data)(attach)(signature)(pub_key))