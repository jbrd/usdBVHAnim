add_component_runtime_library()
add_custom_command(
    TARGET usdBVHPlugin_Shared POST_BUILD
    COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/Public/plugInfo.json $<TARGET_FILE_DIR:usdBVHPlugin_Shared>/plugInfo.json
)
install(FILES $<TARGET_FILE_DIR:usdBVHPlugin_Shared>/plugInfo.json DESTINATION bin)