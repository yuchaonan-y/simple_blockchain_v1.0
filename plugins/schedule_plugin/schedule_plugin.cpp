/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <eosio/schedule_plugin/schedule_plugin.hpp>
#include <fc/exception/exception.hpp>

namespace eosio {
   static appbase::abstract_plugin& _schedule_plugin = app().register_plugin<schedule_plugin>();

class schedule_plugin_impl {
   public:
      int system_cmd(const string cmd, string* rst);
};

schedule_plugin::schedule_plugin():my(new schedule_plugin_impl()){}
schedule_plugin::~schedule_plugin(){}

void schedule_plugin::set_program_options(options_description&, options_description& cfg) {
   cfg.add_options()
         ("option-name", bpo::value<string>()->default_value("default value"),
          "Option Description")
         ;
}

void schedule_plugin::plugin_initialize(const variables_map& options) {
   try {
      if( options.count( "option-name" )) {
         // Handle the option
      }
   }
   FC_LOG_AND_RETHROW()
}

void schedule_plugin::plugin_startup() {
   // Make the magic happen
   string username="";
   if( my->system_cmd("echo -n `whoami`",&username) )
      FC_ASSERT("get username error!");

   string rst="";
   string schedule="crontab -u " + username + " ../../../plugins/schedule_plugin/schedule/schedule.ini";
   if( my->system_cmd(schedule,&rst) )
      FC_ASSERT("get schedule error!");

   ilog("schedule_plugin start!");
}

void schedule_plugin::plugin_shutdown() {
   // OK, that's enough magic
   string username="";
   if( my->system_cmd("echo -n `whoami`",&username) )
      FC_ASSERT("get username error!");
   string rst="";
   string schedule="crontab -u "+ username +" -r";
   if( my->system_cmd(schedule,&rst) )
      FC_ASSERT("delete schedule error!");
}
int schedule_plugin_impl::system_cmd(const string cmd, string* rst){ 
   char line[300];
   FILE *fp;    
   const char *sysCommand = cmd.data(); 
   if ((fp = popen(sysCommand, "r")) == NULL)
   {        
      FC_ASSERT("command run error!");
      return 1;
   }
   while (fgets(line, sizeof(line)-1, fp) != NULL)
   {        
      rst->append(line);
   }    
   pclose(fp);
   return 0;
}
}
