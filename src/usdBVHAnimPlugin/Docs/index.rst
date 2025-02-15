usdBVHAnimPlugin
================

String Parsing
--------------

Parsing of the BVH file format is built on top of a more general purpose string parsing library provided by `Parse.h`.

.. doxygenstruct:: usdBVHAnimPlugin::Parse
   :project: usdBVHAnimPlugin
   :members:
   :no-link:


BVH Structures
--------------

The structures that represent a BVH document are defined in `ParseBVH.h`

.. doxygenenum:: usdBVHAnimPlugin::BVHChannel
   :project: usdBVHAnimPlugin
   :no-link:

.. doxygenstruct:: usdBVHAnimPlugin::BVHOffset
   :project: usdBVHAnimPlugin
   :members:
   :no-link:

.. doxygenstruct:: usdBVHAnimPlugin::BVHTransform
   :project: usdBVHAnimPlugin
   :members:
   :no-link:

.. doxygenstruct:: usdBVHAnimPlugin::BVHDocument
   :project: usdBVHAnimPlugin
   :members:
   :no-link:


BVH Parsing
-----------

The entry points for parsing are defined in `ParseBVH.h`:

.. doxygenfunction:: usdBVHAnimPlugin::ParseBVH(std::string const &filePath, BVHDocument &result)
   :project: usdBVHAnimPlugin

.. doxygenfunction:: usdBVHAnimPlugin::ParseBVH(std::istream& stream, BVHDocument& result)
   :project: usdBVHAnimPlugin

Parsing is implemented in `ParseBVH.cpp`.


USD File Format Plug-in
-----------------------

The plug-in itself is implemented in `BvhFileFormat.cpp`, in which the `BvhFileFormat` class
implements `SdfFileFormat` for the BVH file format. This class has only implemented the **reading**
functionality for BVH files - writing BVH files is not currently supported.

Also note that currently, the entire file is translated and cached in memory at the time the file is opened. BVH data is not currently lazily loaded (e.g. `SdfAbstractData` is not currently implemented for BVH data).

.. doxygenclass:: BvhFileFormat
   :project: usdBVHAnimPlugin
   :members:
   :no-link:
