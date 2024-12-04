# usdBVHAnim

A BVH animation file format plug-in for OpenUSD.

## Build Instructions

### Build Configurations

To build this plugin, the build configuration you choose must match that of your OpenUSD distro,
otherwise you will get compiler and linker errors.

So, to build this plugin for production environments, against a Release build of OpenUSD, please
use `--config Release` (all instructions below will follow this, as its the most common use case).

Likewise, to build this plugin against a Debug build of OpenUSD, please use `--config Debug`.

### Building Locally

* Ensure the toolchain requirements are installed (see below)
* On Linux open a bash shell, on Windows open the x64 Native Tools Command Prompt
* Make a build directory and cd into that (e.g. `mkdir build ; cd build`)
* Generate project files with: `cmake ../`
* Build the project with: `cmake --build ./ -- config Release`
* Build the documentation with: `cmake --build ./ --config Release --target docs`
* Run the tests with: `ctest -C Release ./`
* Validate source code formatting with: `cmake --build ./ --target format-check`
* Automatically format source code with: `cmake --build ./ --target format`

### Testing Locally

* Ensure the toolchain requirements are installed (see below)
* On Linux open a bash shell, on Windows open the x64 Native Tools Command Prompt
* Make a build directory and cd into that (e.g. `mkdir build ; cd build`)
* Specify your deployment directory when generating project files: `cmake -DCMAKE_INSTALL_PREFIX=./deploy ../`
* Install to a local deployment `cmake --build ./ --target install --config Release`
* Run the tests with: `ctest -C Release`
* Add your local deployment to `PXR_PLUGINPATH_NAME`, e.g:
  * `set PXR_PLUGINPATH_NAME=%cd%/deploy/bin` on Windows 
* Run `usdview` to open a .bvh file

## Toolchain Requirements

### Linux and MacOS

* An OpenUSD Installation (see version compatibility table below)
* Git
* CMake
* Clang
* Clang-format
* Valgrind (for tests only)
* Doxygen (documentation only)
* Python 3 (documentation only)
* Sphinx (`pip3 install sphinx`)
* Breathe (`pip3 install breathe`)
* ReadTheDocs Theme for Sphinx (`pip3 install sphinx-rtd-theme`)

### Windows

* An OpenUSD Installation (see version compatibility table below)
* Git for Windows
* Visual Studio 2022 with the following components:
  * C++ Clang Compiler for Windows
  * MSBuild support for LLVM (clang-cl) toolset
  * C++ CMake tools for Windows
* Chocolatey and the following packages:
  * Doxygen (`choco install doxygen.install`)
  * Python 3 (`choco install python3`)
  * Sphinx (`pip3 install sphinx`)
  * Breathe (`pip3 install breathe`)
  * ReadTheDocs Theme for Sphinx (`pip3 install sphinx-rtd-theme`)

## Version Compatibility Table

| Plugin Version      | USD 24.11 |
|---------------------|-----------|
| 1.0.0               | ✅         |

# Contributors

* James Bird (@jbrd)
