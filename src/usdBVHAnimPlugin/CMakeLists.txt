add_component_runtime_library(DESTINATION plugin/usd)
file(GENERATE OUTPUT $<TARGET_FILE_DIR:usdBVHAnimPlugin_Shared>/usdBVHAnim/resources/plugInfo.json
     INPUT ${CMAKE_CURRENT_SOURCE_DIR}/Public/plugInfo.json.in
)
file(GENERATE OUTPUT $<TARGET_FILE_DIR:usdBVHAnimPlugin_Shared>/plugInfo.json
     INPUT ${CMAKE_CURRENT_SOURCE_DIR}/Public/rootPlugInfo.json
)
install(FILES $<TARGET_FILE_DIR:usdBVHAnimPlugin_Shared>/usdBVHAnim/resources/plugInfo.json DESTINATION plugin/usd/usdBVHAnim/resources)
install(FILES $<TARGET_FILE_DIR:usdBVHAnimPlugin_Shared>/plugInfo.json DESTINATION plugin/usd)

# Add additional tests to ensure we can successfully usdcat the test data
add_test(NAME usdBVHAnimPlugin_USDCat_Test COMMAND usdcat --flatten data/test_bvh.bvh WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
add_test(NAME usdBVHAnimPlugin_USDCat_Scale_Test COMMAND usdcat --flatten data/test_bvh_scale_arg.usda WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

# Ensure all test projects run with PXR_PLUGINPATH_NAME pointing at the built artefacts
set_property(TEST usdBVHAnimPlugin_Shared_Tests PROPERTY ENVIRONMENT "PXR_PLUGINPATH_NAME=$<TARGET_FILE_DIR:usdBVHAnimPlugin_Shared>")
set_property(TEST usdBVHAnimPlugin_USDCat_Test PROPERTY ENVIRONMENT "PXR_PLUGINPATH_NAME=$<TARGET_FILE_DIR:usdBVHAnimPlugin_Shared>")
set_property(TEST usdBVHAnimPlugin_USDCat_Scale_Test PROPERTY ENVIRONMENT "PXR_PLUGINPATH_NAME=$<TARGET_FILE_DIR:usdBVHAnimPlugin_Shared>")

if(${VALGRIND} AND VALGRIND_PATH)
     set_property(TEST usdBVHAnimPlugin_Shared_Tests_memcheck PROPERTY ENVIRONMENT "PXR_PLUGINPATH_NAME=$<TARGET_FILE_DIR:usdBVHAnimPlugin_Shared>")
endif()