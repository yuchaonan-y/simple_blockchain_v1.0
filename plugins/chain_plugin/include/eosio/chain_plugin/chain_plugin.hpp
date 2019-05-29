/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once
#include <appbase/application.hpp>
#include <eosio/sqlite_plugin/sqlite_plugin.hpp>
#include <buffer.hpp>

namespace fc
{
FC_DECLARE_EXCEPTION(trx_invalid_arg_exception, invalid_arg_exception_code, "Invalid Argument");
} // namespace fc

namespace eosio
{

using namespace appbase;

template <typename T>
using next_function = std::function<void(const fc::static_variant<fc::exception_ptr, T> &)>;

namespace chain_apis
{
#define TO_REMOVED
typedef std::vector<char> bytes;
struct empty
{
};
class read_only
{
 public:
   struct get_info_results
   {
      std::string info;
   };

   using get_info_params = empty;
   fc::variant get_info(const get_info_params &p);

   using get_block_results = Block;
   struct get_block_params
   {
      uint64_t blk_num;
   };
   fc::variant get_block(const get_block_params &p);

   using get_blocks_results = std::vector<Block>;
   struct get_blocks_params
   {
      std::vector<uint64_t> blk_nums;
   };
   fc::variants get_blocks(const get_blocks_params &p);

   using get_keys_params = empty;
   TO_REMOVED std::vector<fc::variant> get_keys(const get_keys_params &);

   struct hex2char_params
   {
      std::string hex;
   };
   TO_REMOVED std::string hex2char(const hex2char_params &);

   struct char2hex_params
   {
      std::string str;
   };
   TO_REMOVED std::string char2hex(const char2hex_params &);

   struct sign_trx_params
   {
      fc::variant trx;
      std::string key;
   };
   TO_REMOVED fc::variant sign_trx(const sign_trx_params &);

   using check_chain_params = empty;
   fc::variant check_chain(const check_chain_params &p);

   read_only(const Buffer &c) : buf(c){
      sqlite_plugin* plugin = app().find_plugin<sqlite_plugin>();
      this->plugin = plugin;
   };
   ~read_only() = default;
   void validate() const {}

 private:
   const Buffer &buf;
   sqlite_plugin* plugin = nullptr;
};

class read_write
{
 public:
   struct push_transaction_params
   {
      uint32_t type;
      fc::crypto::public_key pub_key;
      fc::crypto::signature signature;
      fc::time_point_sec time_point;
      bytes data;
      bytes attach;
   };
   struct push_transaction_results
   {
      fc::variant res;
   };
   using publish_blk_params = empty;
   void push_transaction(const push_transaction_params &p, next_function<push_transaction_results> next);
   void publish_blk(const publish_blk_params &p, next_function<fc::variant> next);
   read_write(Buffer &buf) : buf(buf){};
   ~read_write() = default;
   void validate() const {}

 private:
   Buffer &buf;
};
} // namespace chain_apis

class chain_plugin : public appbase::plugin<chain_plugin>
{
 public:
   chain_plugin();
   virtual ~chain_plugin();

   APPBASE_PLUGIN_REQUIRES((sqlite_plugin))
   virtual void set_program_options(options_description &, options_description &cfg) override;

   void plugin_initialize(const variables_map &options);
   void plugin_startup();
   void plugin_shutdown();

   const Buffer &chain() const;
   Buffer &chain();

   chain_apis::read_only get_read_only_api() const { return chain_apis::read_only(chain()); }
   chain_apis::read_write get_read_write_api() { return chain_apis::read_write(chain()); }

 private:
   std::unique_ptr<class chain_plugin_impl> my;
};

} // namespace eosio

FC_REFLECT(eosio::chain_apis::empty, )
FC_REFLECT(eosio::chain_apis::read_only::get_block_params, (blk_num))
FC_REFLECT(eosio::chain_apis::read_only::get_blocks_params, (blk_nums))

FC_REFLECT(eosio::chain_apis::read_only::hex2char_params, (hex))
FC_REFLECT(eosio::chain_apis::read_only::char2hex_params, (str))
FC_REFLECT(eosio::chain_apis::read_only::sign_trx_params, (trx)(key))

FC_REFLECT(eosio::chain_apis::read_write::push_transaction_results, (res))
FC_REFLECT(eosio::chain_apis::read_write::push_transaction_params, (type)(data)(attach)(pub_key)(signature)(time_point))
