/**
 *  @file
 *  @copyright defined in eosio/LICENSE.txt
 */
#include <appbase/application.hpp>

#include <eosio/http_plugin/http_plugin.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/chain_api_plugin/chain_api_plugin.hpp>
#include <eosio/sqlite_plugin/sqlite_plugin.hpp>
#include <eosio/schedule_plugin/schedule_plugin.hpp>
#include <fc/exception/exception.hpp>
#include <fc/filesystem.hpp>

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include "config.hpp"

using namespace appbase;
using namespace eosio;


enum return_codes {
   OTHER_FAIL        = -2,
   INITIALIZE_FAIL   = -1,
   SUCCESS           = 0,
   BAD_ALLOC         = 1,
   DATABASE_DIRTY    = 2,
   FIXED_REVERSIBLE  = 3,
   EXTRACTED_GENESIS = 4,
   NODE_MANAGEMENT_SUCCESS = 5
};

int main(int argc, char** argv)
{
   try {
      app().set_version(eosio::nodeos::config::version);

      auto root = fc::app_path();
      app().set_default_data_dir(root / "eosio/nodeos/data" );
      app().set_default_config_dir(root / "eosio/nodeos/config" );
      http_plugin::set_defaults({
         .address_config_prefix = "",
         .default_unix_socket_path = "",
         .default_http_port = 8888
      });
      if(!app().initialize<http_plugin, chain_plugin, chain_api_plugin, sqlite_plugin, schedule_plugin>(argc, argv))
         return INITIALIZE_FAIL;
      //initialize_logging();
      ilog("nodeos version ${ver}", ("ver", app().version_string()));
      ilog("eosio root is ${root}", ("root", root.string()));
      ilog("nodeos using configuration file ${c}", ("c", app().full_config_file_path().string()));
      ilog("nodeos data directory is ${d}", ("d", app().data_dir().string()));
      app().startup();
      app().exec();
   } catch( const fc::exception& e ) {
      if( e.code() == fc::std_exception_code ) {
         if( e.top_message().find( "database dirty flag set" ) != std::string::npos ) {
            elog( "database dirty flag set (likely due to unclean shutdown): replay required" );
            return DATABASE_DIRTY;
         } else if( e.top_message().find( "database metadata dirty flag set" ) != std::string::npos ) {
            elog( "database metadata dirty flag set (likely due to unclean shutdown): replay required" );
            return DATABASE_DIRTY;
         }
      }
      elog( "${e}", ("e", e.to_detail_string()));
      return OTHER_FAIL;
   } catch( const boost::interprocess::bad_alloc& e ) {
      elog("bad alloc");
      return BAD_ALLOC;
   } catch( const boost::exception& e ) {
      elog("${e}", ("e",boost::diagnostic_information(e)));
      return OTHER_FAIL;
   } catch( const std::runtime_error& e ) {
      if( std::string(e.what()) == "database dirty flag set" ) {
         elog( "database dirty flag set (likely due to unclean shutdown): replay required" );
         return DATABASE_DIRTY;
      } else if( std::string(e.what()) == "database metadata dirty flag set" ) {
         elog( "database metadata dirty flag set (likely due to unclean shutdown): replay required" );
         return DATABASE_DIRTY;
      } else {
         elog( "${e}", ("e",e.what()));
      }
      return OTHER_FAIL;
   } catch( const std::exception& e ) {
      elog("${e}", ("e",e.what()));
      return OTHER_FAIL;
   } catch( ... ) {
      elog("unknown exception");
      return OTHER_FAIL;
   }
   return SUCCESS;
}
