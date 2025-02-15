add_component_runtime_library()
file(GENERATE OUTPUT $<TARGET_FILE_DIR:usdBVHAnimPlugin_Shared>/plugInfo.json
     INPUT ${CMAKE_CURRENT_SOURCE_DIR}/Public/plugInfo.json.in
)
install(FILES $<TARGET_FILE_DIR:usdBVHAnimPlugin_Shared>/plugInfo.json DESTINATION bin)
add_test(NAME usdBVHAnimPlugin_USDCat_Test COMMAND usdcat --flatten data/test_bvh.bvh WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
set_property(TEST usdBVHAnimPlugin_USDCat_Test PROPERTY ENVIRONMENT "PXR_PLUGINPATH_NAME=$<TARGET_FILE_DIR:usdBVHAnimPlugin_Shared>")