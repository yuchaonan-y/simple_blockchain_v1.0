set(DB_PATH "${DATA_FOLDER}/blocks.db")
configure_file(config.hpp.ini "include/eoso/sqlite_plugin/config.hpp" ESCAPE_QUOTES @ONLY)
file(GLOB HEADERS "include/eosio/sqlite_plugin/*.hpp")
add_library( sqlite_plugin
             sqlite_plugin.cpp
             ${HEADERS} )
target_link_libraries( sqlite_plugin appbase fc eosio_chain "${CMAKE_SOURCE_DIR}/install/libsqlite3.so")
target_include_directories( sqlite_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" 
 ${CMAKE_CURRENT_BINARY_DIR}/include/eoso/sqlite_plugin 
 "${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/chain/include"
 )