# usdBVHAnim Changelog

## Version 1.1.0

* Added a file format argument to scale BVH animation data at import time. This can be authored by
  specifying the argument when authoring a reference, for example `@./test_bvh.bvh:SDF_FORMAT_ARGS:scale=0.01@`

## Version 1.0.1

* Changed the CMake install target to allow the plug-in to be installed directly into
  an existing USD distro (alternatively, it can still be installed to some separate location).
* No changes to the plug-in itself

## Version 1.0.0

* Initial public version