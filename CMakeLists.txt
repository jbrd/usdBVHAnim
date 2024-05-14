cmake_minimum_required(VERSION 3.28)
include(extern/prefabricated-cmake/main.cmake)
project(
	"usdBVH"
	VERSION "1.0.0"
)
set(PROJECT_AUTHOR "James Bird")

# Find external dependencies
find_package(Boost)
find_package(pxr)

# Set global compile options
if (WIN32)
	add_compile_options(/wd4003)
	# Necessary to ensure that MSVC does not optimize away the SdfFileFormat registration callback
	add_compile_options(/Zc:inline-)
	add_compile_options(/EHsc)
endif()

# Setup common properties accross all targets
function(apply_common_target_properties TARGET)
	set_target_properties(${TARGET} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED true)
endfunction()

function(apply_common_dependencies TARGET)
	target_include_directories(${TARGET} SYSTEM PRIVATE ${PXR_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
	target_link_directories(${TARGET} PRIVATE ${PXR_LIBRARY_DIRS})
	target_link_libraries(${TARGET} ${PXR_LIBRARIES})
endfunction()

# Build all source code components
build_components()