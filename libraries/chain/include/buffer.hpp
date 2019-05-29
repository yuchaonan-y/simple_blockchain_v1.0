#pragma once
#include "block.hpp"
#include "transaction.hpp"
#include <fc/io/json.hpp>
class Buffer
{
  private:
    std::vector<Transaction> trx_pool;
    fc::crypto::public_key producer_key;
    fc::crypto::private_key private_key;
    
    Block last_block;

  public:
    Buffer();
    ~Buffer();
    void cache_last_blk(const Block &blk);
    uint64_t get_catch_trxs_count() const;
    void cache_trx(Transaction &trx);
    Block build_block();
};
