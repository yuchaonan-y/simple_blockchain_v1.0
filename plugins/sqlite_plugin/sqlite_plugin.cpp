/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <eosio/sqlite_plugin/sqlite_plugin.hpp>

namespace eosio {
   static appbase::abstract_plugin& _sqlite_plugin = app().register_plugin<sqlite_plugin>();

class sqlite_plugin_impl {
   public:
      void db_open();
      void db_close();

      fc::optional<Block> get_block(uint64_t num);
      uint64_t get_block_count();
      std::vector<Block> get_all_blocks();
      void append_block(uint64_t id, fc::sha256 hash, std::string json);
   private:
      static int get_block_callback(void *NotUsed, int argc, char **argv, char **azColName);
      static int get_num_callback(void *NotUsed, int argc, char **argv, char **azColName);
   private:
      sqlite3* db;
      static std::vector<Block> tmp_blks;
      static uint64_t blk_nums;
};

std::vector<Block> sqlite_plugin_impl::tmp_blks;
uint64_t sqlite_plugin_impl::blk_nums = 0;

sqlite_plugin::sqlite_plugin():my(new sqlite_plugin_impl()){}
sqlite_plugin::~sqlite_plugin(){}

void sqlite_plugin::set_program_options(options_description&, options_description& cfg) {
   cfg.add_options()
         ("option-name", bpo::value<string>()->default_value("default value"),
          "Option Description")
         ;
}

void sqlite_plugin::plugin_initialize(const variables_map& options) {
   try {
      if( options.count( "option-name" )) {
         // Handle the option
      }
   }
   FC_LOG_AND_RETHROW()
}

void sqlite_plugin::plugin_startup() {
   my->db_open();
   ilog("sqlite_plugin start!");
}

void sqlite_plugin::plugin_shutdown() {
   // OK, that's enough magic
   my->db_close();
}

void sqlite_plugin::append_block(const Block &block){
   std::string json = fc::json::to_string(block);
   my->append_block(block.blk_num, block.digest(), json);
}

fc::optional<Block> sqlite_plugin::get_block(uint64_t num){
   return my->get_block(num);
}

uint64_t sqlite_plugin::get_block_count()
{
   return my->get_block_count();
}

std::vector<Block> sqlite_plugin::get_all_blocks()
{
   return my->get_all_blocks();
}



//////plugin_impl
void sqlite_plugin_impl::db_open(){
   int ret = sqlite3_open(DB_PATH, &db);
   dlog("db path=${p}", ("p", DB_PATH));
   if (ret) {
      elog("error happened: ${e}", ("e", sqlite3_errmsg(db)));
   }
}

void sqlite_plugin_impl::db_close(){
   sqlite3_close(db);
}

int sqlite_plugin_impl::get_block_callback(void *NotUsed, int argc, char **argv, char **azColName){

   for(int i = 0; i < argc; i++){
      dlog("${n} = ${v}", ("n", azColName[i])("v", argv[i] ? argv[i] : "NULL"));
      if (std::string(azColName[i]) == "JSON") {
         FC_ASSERT(argv[i], "can not get JSON value!");
         std::string json = argv[i];
         fc::variant v = fc::json::from_string(json);
         Block blk;
         fc::from_variant(v, blk);
         tmp_blks.push_back(blk);
      }
      dlog(" ${b}", ("b", tmp_blks));
   }
   return 0;
}

int sqlite_plugin_impl::get_num_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
      for(int i = 0; i < argc; i++){
      dlog("${n} = ${v}", ("n", azColName[i])("v", argv[i] ? argv[i] : "NULL"));
      if (std::string(azColName[i]) == "num") {
         FC_ASSERT(argv[i], "can not get num value!");
         std::stringstream ss;
         ss << argv[i];
         ss >> blk_nums;
      }
      dlog(" block nums =${b}", ("b", blk_nums));
   }
   return 0;
}

void sqlite_plugin_impl::append_block(uint64_t id, fc::sha256 hash, std::string json){
   char* zErrMsg = NULL;
   std::string sql = "INSERT INTO BLOCK (ID,HASH,JSON) "  \
         "VALUES(" + std::to_string(id) + ",'" + std::string(hash) + "','" + json + "');";
   dlog("sql = ${s}", ("s", sql));
   int rc = sqlite3_exec(db, sql.c_str(), get_block_callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      elog("SQL error: ${e}\n", ("e", zErrMsg));
      sqlite3_free(zErrMsg);
   }else{
      dlog("Records created successfully");
   }
}

fc::optional<Block> sqlite_plugin_impl::get_block(uint64_t num){
   char* zErrMsg = NULL;
   std::string sql = "SELECT JSON FROM BLOCK WHERE ID=" + std::to_string(num) + ";";
   dlog("sql = ${s}", ("s", sql));
   tmp_blks.clear();
   int rc = sqlite3_exec(db, sql.c_str(), get_block_callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      elog("SQL error: ${e}\n", ("e", zErrMsg));
      sqlite3_free(zErrMsg);
      return fc::optional<Block>();
   }else{
      dlog("Records created successfully");
   }
   if (tmp_blks.size() == 0) {
      return fc::optional<Block>();

   }
   return fc::optional<Block>(tmp_blks.at(0));
}

uint64_t sqlite_plugin_impl::get_block_count()
{
   char* zErrMsg = NULL;
   blk_nums = 0;
   std::string sql = "select count (*) as num from block;";
   int rc = sqlite3_exec(db, sql.c_str(), get_num_callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      elog("SQL error: ${e}\n", ("e", zErrMsg));
      sqlite3_free(zErrMsg);
   }else{
      dlog("Records created successfully");
   }
   return blk_nums;
}

std::vector<Block> sqlite_plugin_impl::get_all_blocks(){
   char* zErrMsg = NULL;
   std::string sql = "SELECT JSON FROM BLOCK ORDER BY ID;";
   tmp_blks.clear();
   int rc = sqlite3_exec(db, sql.c_str(), get_block_callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      elog("SQL error: ${e}\n", ("e", zErrMsg));
      sqlite3_free(zErrMsg);
      return std::vector<Block>();
   }else{
      dlog("Records created successfully");
   }
   return tmp_blks;
}

}
