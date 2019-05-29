add_subdirectory(http_plugin)
add_subdirectory(chain_plugin)
add_subdirectory(chain_api_plugin)
add_subdirectory(sqlite_plugin)
add_subdirectory(tool_plugin)
add_subdirectory(schedule_plugin)

# Forward variables to top level so packaging picks them up
set(CPACK_DEBIAN_PACKAGE_DEPENDS ${CPACK_DEBIAN_PACKAGE_DEPENDS} PARENT_SCOPE)
