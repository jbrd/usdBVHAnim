Example USD Structure
=====================

Overview
--------

Since a BVH animation file only contains skeletal animation data, the plug-in will translate
a BVH file into a hierarchy of the following prims from the `usdSkel` schema:

* A ``SkelRoot`` prim containing:

   * A ``Skeleton`` prim that holds the bone heirarchy of the BVH file

   * A ``SkelAnimation`` prim that holds the skeletal animation data from the BVH file

Timing information is encoded into the metadata of the root layer, and relationships are set up to ensure
the animation data drives the skeleton by default.


A Note About Extents
--------------------

As recommended, extents for the BVH animation data are computed by the plug-in so that the resultant
``skelRoot`` contains extents that are valid at the time of authoring.

However, if later on in the pipeline, you plan on composing BVH animation data into a larger scene, and in particular, if you plan on either:

* Composing skinned meshes into the ``skelRoot`` and binding them to animation data
* Composing the BVH data into an existing ``skelRoot``

then, these extents should be re-authored so that they reflect every prim that ends up parented to the ``skelRoot``.


Example
-------

To illustrate the prims produced by the plug-in, the following example is a direct result of running
``usdcat --flattenLayerStack data\test_bvh.bvh``:

.. code-block::

    #usda 1.0
    (
        defaultPrim = "Root"
        endTimeCode = 21
        startTimeCode = 1
        timeCodesPerSecond = 23.999808001535985
    )

    def SkelRoot "Root" (
        prepend apiSchemas = ["SkelBindingAPI"]
    )
    {
        float3[] extent.timeSamples = {
            1: [(0, 0, 0), (0, 0, 1)],
            2: [(0, -0.004576359, 0), (0, 0.008019, 0.99992067)],
            ...
        }
        prepend rel skel:skeleton = </Root/Skeleton>

        def Skeleton "Skeleton" (
            prepend apiSchemas = ["SkelBindingAPI"]
        )
        {
            uniform matrix4d[] bindTransforms = [( (1, 0, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0), (0, 0, 0, 1) ), ( (1, 0, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0), (0, 0, 1, 1) )]
            uniform token[] joints = ["Root", "Root/Foo"]
            uniform matrix4d[] restTransforms = [( (1, 0, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0), (0, 0, 0, 1) ), ( (1, 0, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0), (0, 0, 1, 1) )]
            prepend rel skel:animationSource = </Root/Animation>
        }

        def SkelAnimation "Animation"
        {
            uniform token[] joints = ["Root", "Root/Foo"]
            quatf[] rotations.timeSamples = {
                1: [(1, 0, 0, 0), (1, 0, 0, 0)],
                2: [(0.99998015, 0.0062978044, 0, 0), (0.99998015, 0, 0.0062978044, 0)],
                ...
            }
            half3[] scales.timeSamples = {
                1: [(1, 1, 1), (1, 1, 1)],
            }
            float3[] translations.timeSamples = {
                1: [(0, 0, 0), (0, 0, 1)],
                2: [(0, 0.008019, 0), (0, 0, 1)],
                ...
            }
        }
    }