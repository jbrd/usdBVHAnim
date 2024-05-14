# usdBVH

Bla.

## Build Instructions

### Building Locally

* Ensure the toolchain requirements are installed (see below)
* On Linux open a bash shell, on Windows open the x64 Native Tools Command Prompt
* cd into the example directory (e.g. `cd example`)
* Make a build directory and cd into that (e.g. `mkdir build ; cd build`)
* Generate project files with: `cmake ../`
* Build the project with: `cmake --build ./`
* Build the documentation with: `cmake --build ./ --target docs`
* Run the tests with: `ctest -C Debug ./`
* Format source code with: `cmake --build ./ --target format`

### Testing Locally

* Specify your deployment directory when generating project files: `cmake -DCMAKE_INSTALL_PREFIX=./deploy ../`
* Build the project in Release mode with: `cmake --build ./ --config Release`
* Build the documentation: `cmake --build ./ --target docs`
* Install to a local deployment `cmake --build ./ --target install`
* Add your local deployment to `PXR_PLUGINPATH_NAME`, e.g: `set PXR_PLUGINPATH_NAME=%cd%/deploy/bin`
* Run usdview to open a .bvh file


## Toolchain Requirements

### Linux

* Git
* CMake
* Clang
* Clang-format
* Valgrind
* Doxygen
* Python 3
* Sphinx (`pip3 install sphinx`)
* Breathe (`pip3 install breathe`)
* ReadTheDocs Theme for Sphinx (`pip3 install sphinx-rtd-theme`)

### Windows

* Visual Studio 2019 with the following components:
  * C++ Clang Compiler for Windows (12.0.0 or above)
  * C++ Clang-cl for v142 build tools
  * C++ CMake tools for Windows
* Git for Windows
* Chocolatey and the following packages:
  * Doxygen (`choco install doxygen.install`)
  * Python 3 (`choco install python3`)
  * Sphinx (`pip3 install sphinx`)
  * Breathe (`pip3 install breathe`)
  * ReadTheDocs Theme for Sphinx (`pip3 install sphinx-rtd-theme`)