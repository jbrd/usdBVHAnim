cmake_minimum_required(VERSION 3.28)
include(extern/prefabricated-cmake/main.cmake)
project(
	"usdBVHAnim"
	VERSION "1.0.0"
)
set(PROJECT_AUTHOR "James Bird")

# Find external dependencies
find_package(Boost)
find_package(OpenGL)
find_package(pxr REQUIRED)
if (UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
endif()

# Set global compile options
add_compile_definitions(_USE_MATH_DEFINES)
add_compile_definitions(
    TBB_USE_ASSERT
    TBB_USE_THREADING_TOOLS
    TBB_SUPPRESS_DEPRECATED_MESSAGES
    TBB_USE_PERFORMANCE_WARNINGS
)

if(WIN32)
	add_compile_options(/wd4003)
	# Necessary to ensure that MSVC does not optimize away the SdfFileFormat registration callback
	add_compile_options(/Zc:inline-)
	add_compile_options(/permissive-)
	add_compile_options(/Zc:rvalueCast)
	add_compile_options(/Zc:strictStrings)
	add_compile_options(/EHsc)
	add_compile_options(/DNOMINMAX)
else()
	add_compile_options(-Wno-deprecated -Wno-deprecated-declarations -Wno-gnu-zero-variadic-macro-arguments)
endif()

# Setup common properties accross all targets
function(apply_common_target_properties TARGET)
	set_target_properties(${TARGET} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED true)
	target_include_directories(${TARGET} SYSTEM PRIVATE ${PXR_INCLUDE_DIRS})
	if(${Boost_FOUND})
		target_include_directories(${TARGET} SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
	endif()
	target_link_directories(${TARGET} PRIVATE ${PXR_LIBRARY_DIRS})
	target_link_libraries(${TARGET} ${PXR_LIBRARIES})
endfunction()

# Build all source code components
build_components()