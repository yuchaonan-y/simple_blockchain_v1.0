/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once
#include <appbase/application.hpp>
#include <fc/exception/exception.hpp>
#include <fc/crypto/public_key.hpp>
#include <fc/crypto/private_key.hpp>
#include <fc/crypto/signature.hpp>
#include <fc/io/json.hpp>
#include <fc/crypto/hex.hpp>
#include <transaction.hpp>
namespace eosio {

using namespace appbase;

/**
 *  This is a template plugin, intended to serve as a starting point for making new plugins
 */
class tool_plugin : public appbase::plugin<tool_plugin> {
public:
   tool_plugin();
   virtual ~tool_plugin();
 
   APPBASE_PLUGIN_REQUIRES()
   virtual void set_program_options(options_description&, options_description& cfg) override;
 
   void plugin_initialize(const variables_map& options);
   void plugin_startup();
   void plugin_shutdown();

private:
   void generate_key();
   void hex_to_string();
   void string_to_hex();
   void sign_transaction();
   bool check_chain();
   
private:
   int type;// 1-generate-key   2-hex-to-string 3-string-hex 4-sign-transaction
   std::vector<std::string> option_datas;//1-none 2-hex 3-string 4-trx-json
   std::unique_ptr<class tool_plugin_impl> my;
};

}
