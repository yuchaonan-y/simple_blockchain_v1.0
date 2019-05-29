#include "buffer.hpp"

Buffer::Buffer()
//FIX ME
:producer_key(std::string("EOS6hNSYPHaJm1TBmrLHLytxjwEcX6gbVNRZe4o7DxcVL7ojH9PCZ"))
,private_key(std::string("5HwLWM8J46qbpZDMYsSGDxWN9EkusGwGP1EH5eFJKYvasuW83yX"))
{
}

Buffer::~Buffer()
{
}

void Buffer::cache_last_blk(const Block& blk)
{
    this->last_block = blk;
}

uint64_t Buffer::get_catch_trxs_count() const
{
    return trx_pool.size();
}

void Buffer::cache_trx(Transaction& trx)
{
    trx_pool.push_back(trx);
}

Block Buffer::build_block()
{
    Block blk;
    dlog("last block = ${l}", ("l", last_block));
    dlog("if valid = ${l}", ("l", last_block.valid()));
    dlog("last_block.blk_num  = ${l}", ("l", (last_block.blk_num )));
    if ( last_block.valid() ) 
    {
        blk.prior_hash = last_block.digest();
        blk.blk_num = last_block.blk_num + 1;
    }
    else
    {
        blk.prior_hash = fc::sha256();
        blk.blk_num = 0;
    }
    blk.push_trxs(trx_pool);
    blk.sig = private_key.sign(blk.digest());
    trx_pool.clear();
    dlog("new block = ${j}",("j", fc::json::to_string(blk)));
    return blk;
}
