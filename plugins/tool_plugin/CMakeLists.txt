file(GLOB HEADERS "include/eosio/tool_plugin/*.hpp")
add_library( tool_plugin
             tool_plugin.cpp
             ${HEADERS} )

target_link_libraries( tool_plugin sqlite_plugin eosio_chain appbase fc )
target_include_directories( tool_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
