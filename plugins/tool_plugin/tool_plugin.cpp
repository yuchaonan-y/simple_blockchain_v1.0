/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <eosio/tool_plugin/tool_plugin.hpp>
#include <eosio/sqlite_plugin/sqlite_plugin.hpp>

namespace eosio
{
static appbase::abstract_plugin &_tool_plugin = app().register_plugin<tool_plugin>();

class tool_plugin_impl
{
 public:
};

tool_plugin::tool_plugin() : my(new tool_plugin_impl()) {}
tool_plugin::~tool_plugin() {}

void tool_plugin::set_program_options(options_description &, options_description &cfg)
{
   cfg.add_options()("keygen", bpo::value<string>(),
                     "Generate key");
   cfg.add_options()("h2c", bpo::value<string>(),
                     "hex char to ascii char");
   cfg.add_options()("c2h", bpo::value<string>(),
                     "ascii char to hex char");
   cfg.add_options()("sign", bpo::value<string>(),
                     "sign transaction");
   cfg.add_options()("key", bpo::value<string>(),
                     "the private key used to sign transaction");
   cfg.add_options()("chkchain", bpo::value<string>(),
                     "check whether the chain was changed");
}

void tool_plugin::plugin_initialize(const variables_map &options)
{
   try
   {
      if (options.count("keygen"))
      {
         type = 1;
      }
      else if (options.count("h2c"))
      {
         type = 2;
         option_datas.push_back(options["h2c"].as<string>());
      }
      else if (options.count("c2h"))
      {
         type = 3;
         option_datas.push_back(options["c2h"].as<string>());
      }
      else if (options.count("sign") && options.count("key"))
      {
         type = 4;
         option_datas.push_back(options["sign"].as<string>());
         option_datas.push_back(options["key"].as<string>());
      }
      else if (options.count("chkchain") )
      {
         type = 5;
      }
   }
   FC_LOG_AND_RETHROW()
}

void tool_plugin::plugin_startup()
{
   switch (type)
   {
      case 1:
         generate_key();
         break;
      case 2:
         hex_to_string();
         break;
      case 3:
         string_to_hex();
         break;
      case 4:
         sign_transaction();
         break;
      case 5:
         check_chain();
         break;
      default:
         FC_ASSERT(false);
         break;
   }
   ilog("tool_plugin start!");
}

void tool_plugin::plugin_shutdown()
{
   // OK, that's enough magic
}

void tool_plugin::generate_key()
{
   using namespace fc::crypto;
   using namespace fc;
   auto key = private_key::generate<ecc::private_key_shim>();
   auto pub = key.get_public_key();
   dlog("PUBLIC KEY: ${pub}\nPRIVATE KEY: ${pri}\n", ("pub", pub)("pri", key));
}

void tool_plugin::hex_to_string()
{
   using namespace fc::crypto;
   using namespace fc;
   string hex = option_datas.at(0);
   char* out_data = new char[1024]{0};
   size_t len = from_hex(hex, out_data, 1024);
   dlog("ASCII CHARS: ${c}\n", ("c", out_data));
   delete[] out_data;
}

void tool_plugin::string_to_hex()
{
   using namespace fc::crypto;
   using namespace fc;
   std::string str = option_datas.at(0);
   std::string hex = fc::to_hex(str.c_str(), str.size());
   dlog("HEX CHARS: ${h}\n",("h", hex));
}

void tool_plugin::sign_transaction()
{
   using namespace fc::crypto;
   using namespace fc;
   std::string json = option_datas.at(0);
   std::string key = option_datas.at(1);
   Transaction tx;
   variant v = json::from_string(json);
   from_variant(v, tx);
   sha256 digest = tx.digest();
   private_key pri_key(key);
   tx.signature = pri_key.sign(digest);
   tx.pub_key = pri_key.get_public_key();
  // to_variant(tx,v);
   dlog("SIGNED TRANSACTION: ${j}\n", ("j", json::to_string(tx)));
}

bool tool_plugin::check_chain()
{
   Block checkBlk;
   sqlite_plugin* plugin = app().find_plugin<sqlite_plugin>();
   auto blocks = plugin->get_all_blocks();
      
   for(auto idx : blocks )
   {
      if ( idx.blk_num == 0 ) 
      {
         checkBlk = idx ;
         continue;
      } 

      if ( ! idx.check_block( checkBlk ) )
      {
            dlog("ERROR: block prior_hash are different with ${j}\n", ("j", checkBlk));
            return false;
      }
      checkBlk = idx ;
   }
   dlog("Success check all chain \n");
   //成功返回标志0，默认成功返回块0
   return true;
}

} // namespace eosio
