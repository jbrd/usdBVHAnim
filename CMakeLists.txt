cmake_minimum_required(VERSION 3.12)
include(extern/prefabricated-cmake/main.cmake)
project(
	"usdBVH"
	VERSION "1.0.0"
)
set(PROJECT_AUTHOR "James Bird")

# Find external dependencies
find_package(Boost)
find_package(pxr)

# Called for all targets to apply common dependencies
function(apply_common_dependencies TARGET)
	message("Add dependencies to: ${TARGET}")
endfunction()

# Build all source code components
build_components()