#include "block.hpp"

Block::Block()
{
    time_point = fc::time_point::now();
}

fc::sha256 Block::digest() const
{
    fc::sha256::encoder enc;
    fc::raw::pack(enc, blk_num);
    fc::raw::pack(enc, prior_hash);
    fc::raw::pack(enc, time_point);
    fc::raw::pack(enc, merkle_root);
    return enc.result();
}

bool Block::valid() const
{
    if (fc::crypto::signature() == sig) {
        wlog("a blank signature");
        return false;
    }
    auto recover_key = fc::crypto::public_key(sig, digest());
    //FIXME
    fc::crypto::public_key pub_key(std::string("EOS6hNSYPHaJm1TBmrLHLytxjwEcX6gbVNRZe4o7DxcVL7ojH9PCZ"));
    dlog("signature = ${sig} ,recover key = ${rec}, public key = ${pub}", ("sig", sig)("rec", recover_key)("pub", pub_key)); 
    return recover_key == pub_key;
}

void Block::push_trxs(std::vector<Transaction>& trxs)
{
    this->trxs.assign(trxs.cbegin(), trxs.cend());
    //FIXME  the merkle root hash may be not right
    merkle_root = fc::sha256::hash(trxs);
}

bool Block::check_block(const Block &piror_blk )
{
    if ( piror_blk.valid() && this->valid())
    {
        if ( piror_blk.digest() == this->prior_hash )
        {
            //当前块中的prior_hash与上一块对应的hash值不符，则说明上一块被修改，报错
            return true;
        }
    }
    return false;
}