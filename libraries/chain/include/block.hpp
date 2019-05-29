#pragma once
#include <fc/crypto/sha256.hpp>
#include "transaction.hpp"

class Block{

public:
    Block();
    fc::sha256 digest() const;
    bool valid() const;

    uint64_t blk_num;
    fc::sha256  prior_hash;
    fc::time_point_sec time_point;
    fc::sha256 merkle_root;
    fc::crypto::signature sig;
    std::vector<Transaction> trxs;

    void push_trxs(std::vector<Transaction>& trxs);
    bool check_block(const Block &piror_blk);
};

FC_REFLECT(Block, (time_point)(prior_hash)(blk_num)(trxs)(merkle_root)(sig))