Building and Installing
=======================

Version Compatibility Table
---------------------------

First, make sure you are building a version of the plug-in that is compatible with
the version of USD you are targetting:

.. list-table::
   :widths: 25 25
   :header-rows: 1

   * - Plugin Version
     - USD 24.11
   * - 1.0.0
     - ✅


Toolchain Requirements
----------------------

Next, make sure you have the following software installed:

Linux and MacOS
^^^^^^^^^^^^^^^

* An OpenUSD Installation (see version compatibility table)
* Git
* CMake
* Clang
* Clang-format
* Valgrind (for tests only)
* Doxygen (documentation only)
* Python 3 (documentation only)
* Sphinx (``pip3 install sphinx``)
* Breathe (``pip3 install breathe``)
* ReadTheDocs Theme for Sphinx (``pip3 install sphinx-rtd-theme``)

Windows
^^^^^^^

* An OpenUSD Installation (see version compatibility table)
* Git for Windows
* Visual Studio 2022 with the following components:

   * C++ Clang Compiler for Windows
   * MSBuild support for LLVM (clang-cl) toolset
   * C++ CMake tools for Windows

* Chocolatey and the following packages:

   * Doxygen (``choco install doxygen.install``)
   * Python 3 (``choco install python3``)
   * Sphinx (``pip3 install sphinx``)
   * Breathe (``pip3 install breathe``)
   * ReadTheDocs Theme for Sphinx (``pip3 install sphinx-rtd-theme``)


Build Instructions
------------------

Build Configurations
^^^^^^^^^^^^^^^^^^^^

To build this plugin, the build configuration you choose must match that of your OpenUSD distro,
otherwise you will get compiler and linker errors.

So, to build this plugin for production environments, against a Release build of OpenUSD, please
use ``--config Release`` (all instructions below will follow this, as its the most common use case).

Likewise, to build this plugin against a Debug build of OpenUSD, please use ``--config Debug``.

Building Locally
^^^^^^^^^^^^^^^^

* On Linux open a bash shell, on Windows open the x64 Native Tools Command Prompt
* Make a build directory and cd into that (e.g. ``mkdir build ; cd build``)
* Generate project files with: ``cmake ../``
* Build the project with: ``cmake --build ./ -- config Release``
* Build the documentation with: ``cmake --build ./ --config Release --target docs``
* Run the tests with: ``ctest -C Release ./``
* Validate source code formatting with: ``cmake --build ./ --target format-check``
* Automatically format source code with: ``cmake --build ./ --target format``

Testing Locally
^^^^^^^^^^^^^^^

* On Linux open a bash shell, on Windows open the x64 Native Tools Command Prompt
* Make a build directory and cd into that (e.g. ``mkdir build ; cd build``)
* Specify your deployment directory when generating project files: ``cmake -DCMAKE_INSTALL_PREFIX=./deploy ../``
* Install to a local deployment ``cmake --build ./ --target install --config Release``
* Add your local deployment to ``PXR_PLUGINPATH_NAME``, e.g: ``set PXR_PLUGINPATH_NAME=%cd%/deploy/bin`` on Windows 
* Run ``usdview`` to open a .bvh file

Permanently Installing The Plug-in
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* Specify a permanent location for the plug-in when configuring the build, e.g: ``cmake -DCMAKE_INSTALL_PREFIX=/opt/usd/plugins``
* Make a build directory and cd into that (e.g. ``mkdir build ; cd build``)
* Install with: ``cmake --build ./ --target install --config Release``
* Ensure your environment always adds your chosen directory's bin folder to ``PXR_PLUGINPATH_NAME``, e.g: ``PXR_PLUGINPATH_NAME=/opt/usd/plugins/bin``


At this point, the plug-in should be available whenever you use USD.
