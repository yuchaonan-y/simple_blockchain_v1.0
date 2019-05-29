file(GLOB HEADERS "include/*.hpp" )
## SORT .cpp by most likely to change / break compile
add_library( eosio_chain
             transaction.cpp
             block.cpp
             buffer.cpp
             ${HEADERS}
             )

target_link_libraries( eosio_chain fc  )
#softfloat builtins
target_include_directories( eosio_chain
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_BINARY_DIR}/include"
                            "${CMAKE_SOURCE_DIR}/libraries/fc/include"
                            )