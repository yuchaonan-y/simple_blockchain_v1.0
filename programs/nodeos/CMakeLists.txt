add_executable( ${NODE_EXECUTABLE_NAME} main.cpp )

if( UNIX AND NOT APPLE )
  set(rt_library rt )
endif()

find_package( Gperftools QUIET )
if( GPERFTOOLS_FOUND )
    message( STATUS "Found gperftools; compiling ${NODE_EXECUTABLE_NAME} with TCMalloc")
    list( APPEND PLATFORM_SPECIFIC_LIBS tcmalloc )
endif()


SET(nodeos_BUILD_VERSION 0)
configure_file(config.hpp.in config.hpp ESCAPE_QUOTES)

target_include_directories(${NODE_EXECUTABLE_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

if(UNIX)
  if(APPLE)
    set(whole_archive_flag "-force_load")
    set(no_whole_archive_flag "")
    set(build_id_flag "")
  else()
    set(whole_archive_flag "--whole-archive")
    set(no_whole_archive_flag "--no-whole-archive")
    set(build_id_flag "--build-id")
  endif()
else()
  set(whole_archive_flag "--whole-archive")
  set(no_whole_archive_flag "--no-whole-archive")
  set(build_id_flag "")
endif()

target_link_libraries( ${NODE_EXECUTABLE_NAME}
        PRIVATE appbase
#        PRIVATE -Wl,${whole_archive_flag} login_plugin               -Wl,${no_whole_archive_flag}
#        PRIVATE -Wl,${whole_archive_flag} history_plugin             -Wl,${no_whole_archive_flag}
#        PRIVATE -Wl,${whole_archive_flag} state_history_plugin       -Wl,${no_whole_archive_flag}
#        PRIVATE -Wl,${whole_archive_flag} bnet_plugin                -Wl,${no_whole_archive_flag}
#        PRIVATE -Wl,${whole_archive_flag} history_api_plugin         -Wl,${no_whole_archive_flag}
#        PRIVATE -Wl,${whole_archive_flag} chain_api_plugin           -Wl,${no_whole_archive_flag}
#        PRIVATE -Wl,${whole_archive_flag} net_plugin                 -Wl,${no_whole_archive_flag}
#        PRIVATE -Wl,${whole_archive_flag} net_api_plugin             -Wl,${no_whole_archive_flag}
#        PRIVATE -Wl,${whole_archive_flag} faucet_testnet_plugin      -Wl,${no_whole_archive_flag}
#        PRIVATE -Wl,${whole_archive_flag} txn_test_gen_plugin        -Wl,${no_whole_archive_flag}
#        PRIVATE -Wl,${whole_archive_flag} db_size_api_plugin         -Wl,${no_whole_archive_flag}
#        PRIVATE -Wl,${whole_archive_flag} producer_api_plugin        -Wl,${no_whole_archive_flag}
#        PRIVATE -Wl,${whole_archive_flag} test_control_plugin        -Wl,${no_whole_archive_flag}
#        PRIVATE -Wl,${whole_archive_flag} test_control_api_plugin    -Wl,${no_whole_archive_flag}
#        PRIVATE -Wl,${build_id_flag}
        PRIVATE  http_plugin  chain_plugin chain_api_plugin sqlite_plugin schedule_plugin
        PRIVATE  fc ${CMAKE_DL_LIBS} ${PLATFORM_SPECIFIC_LIBS} )