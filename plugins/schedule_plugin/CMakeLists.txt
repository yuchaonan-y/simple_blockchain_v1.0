file(GLOB HEADERS "include/eosio/schedule_plugin/*.hpp")
add_library( schedule_plugin
             schedule_plugin.cpp
             ${HEADERS} )

target_link_libraries( schedule_plugin appbase fc )
target_include_directories( schedule_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
execute_process(COMMAND sh ../plugins/schedule_plugin/movescript.sh ../plugins/schedule_plugin)