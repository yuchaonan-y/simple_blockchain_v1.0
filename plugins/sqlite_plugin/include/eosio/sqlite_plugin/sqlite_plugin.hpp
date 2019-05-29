/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once
#include <appbase/application.hpp>
#include <fc/exception/exception.hpp>
#include <fc/log/logger.hpp>
#include <fc/io/json.hpp>
#include <block.hpp>
#include "sqlite3.h"
#include "config.hpp"
namespace eosio
{

using namespace appbase;

/**
 *  This is a template plugin, intended to serve as a starting point for making new plugins
 */
class sqlite_plugin : public appbase::plugin<sqlite_plugin>
{
 public:
   sqlite_plugin();
   virtual ~sqlite_plugin();

   APPBASE_PLUGIN_REQUIRES()
   virtual void set_program_options(options_description &, options_description &cfg) override;

   void plugin_initialize(const variables_map &options);
   void plugin_startup();
   void plugin_shutdown();

   void append_block(const Block &block);
   fc::optional<Block> get_block(uint64_t num);
   std::vector<Block> get_all_blocks();
   uint64_t get_block_count();

   
 private:
   std::unique_ptr<class sqlite_plugin_impl> my;
};

} // namespace eosio
