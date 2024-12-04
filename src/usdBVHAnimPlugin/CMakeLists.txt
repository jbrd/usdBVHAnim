add_component_runtime_library()
add_custom_command(
    TARGET usdBVHAnimPlugin_Shared POST_BUILD
    COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/Public/plugInfo.json $<TARGET_FILE_DIR:usdBVHAnimPlugin_Shared>/plugInfo.json
)
install(FILES $<TARGET_FILE_DIR:usdBVHAnimPlugin_Shared>/plugInfo.json DESTINATION bin)
add_test(NAME usdBVHAnimPlugin_USDCat_Test COMMAND usdcat --flatten data/test_bvh.bvh WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
set_property(TEST usdBVHAnimPlugin_USDCat_Test PROPERTY ENVIRONMENT "PXR_PLUGINPATH_NAME=$<TARGET_FILE_DIR:usdBVHAnimPlugin_Shared>")