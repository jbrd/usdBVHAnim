#include "Parse.h"
#include "Tests.h"
#include <pxr/base/vt/array.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdSkel/animation.h>
#include <pxr/usd/usdSkel/root.h>
#include <pxr/usd/usdSkel/skeleton.h>

using namespace usdBVHAnimPlugin;

BEGIN_TEST_FIXTURE(USDTests)

TEST(BvhFileFormatPlugin_WithoutScaleFileFormatArg_AppliesExpectedScale)
{
    // Expected no applied scale here...
    auto stage = pxr::UsdStage::Open("data/test_bvh_without_scale_arg.usda");
    TEST_REQUIRE(stage);

    // Ensure we have the expected skel root
    auto skelRoot = pxr::UsdSkelRoot(stage->GetPrimAtPath(pxr::SdfPath("/Root")));
    TEST_REQUIRE(skelRoot);

    // Ensure the bind pose and rest pose transforms on the Skeleton haven't been scaled
    auto skeleton = pxr::UsdSkelSkeleton(stage->GetPrimAtPath(pxr::SdfPath("/Root/Skeleton")));
    TEST_REQUIRE(skeleton);

    pxr::GfMatrix4d const expectedRootTransform(1.0f);
    pxr::GfMatrix4d const expectedBoneTransform(pxr::GfMatrix3d(1.0f), pxr::GfVec3d(0.0f, 0.0f, 1.0f));

    pxr::VtArray<pxr::GfMatrix4d> bindTransformsLS;
    skeleton.GetBindTransformsAttr().Get(&bindTransformsLS);
    TEST_REQUIRE(pxr::GfIsClose(expectedRootTransform, bindTransformsLS[0], 1e-4));
    TEST_REQUIRE(pxr::GfIsClose(expectedBoneTransform, bindTransformsLS[1], 1e-4));

    pxr::VtArray<pxr::GfMatrix4d> restTransformsLS;
    skeleton.GetRestTransformsAttr().Get(&restTransformsLS);
    TEST_REQUIRE(pxr::GfIsClose(expectedRootTransform, restTransformsLS[0], 1e-4));
    TEST_REQUIRE(pxr::GfIsClose(expectedBoneTransform, restTransformsLS[1], 1e-4));

    // Ensure the animation has been correctly rescaled
    auto animation = pxr::UsdSkelAnimation(stage->GetPrimAtPath(pxr::SdfPath("/Root/Animation")));
    TEST_REQUIRE(animation);
    {
        // Translation on leaf bone should be of length 1
        std::vector<double> timeSamples;
        animation.GetTranslationsAttr().GetTimeSamples(&timeSamples);

        for (auto timeSample : timeSamples) {
            pxr::VtArray<pxr::GfVec3f> translations;
            animation.GetTranslationsAttr().Get(&translations, timeSample);
            TEST_REQUIRE(translations.size() == 2);
            TEST_REQUIRE(pxr::GfIsClose(translations[1].GetLength(), 1.0f, 1e-4f));
        }
    }
    {
        // Scale should remain at 1.0
        std::vector<double> timeSamples;
        animation.GetScalesAttr().GetTimeSamples(&timeSamples);

        for (auto timeSample : timeSamples) {
            pxr::VtArray<pxr::GfVec3h> scales;
            animation.GetScalesAttr().Get(&scales, timeSample);
            TEST_REQUIRE(scales.size() == 2);
            TEST_REQUIRE(pxr::GfIsClose(scales[0], pxr::GfVec3h(1.0f, 1.0f, 1.0f), 1e-4f));
            TEST_REQUIRE(pxr::GfIsClose(scales[1], pxr::GfVec3h(1.0f, 1.0f, 1.0f), 1e-4f));
        }
    }
}

TEST(BvhFileFormatPlugin_WithScaleFileFormatArg_AppliesExpectedScale)
{
    // Expecting this to empart a scale of 0.01
    auto stage = pxr::UsdStage::Open("data/test_bvh_scale_arg.usda");
    TEST_REQUIRE(stage);

    // Ensure we have the expected skel root
    auto skelRoot = pxr::UsdSkelRoot(stage->GetPrimAtPath(pxr::SdfPath("/Root")));
    TEST_REQUIRE(skelRoot);

    // Ensure the bind pose and rest pose transforms on the Skeleton are correctly scaled
    auto skeleton = pxr::UsdSkelSkeleton(stage->GetPrimAtPath(pxr::SdfPath("/Root/Skeleton")));
    TEST_REQUIRE(skeleton);

    pxr::GfMatrix4d const expectedRootTransform(1.0f);
    pxr::GfMatrix4d const expectedBoneTransform(pxr::GfMatrix3d(1.0f), pxr::GfVec3d(0.0f, 0.0f, 0.01f));

    pxr::VtArray<pxr::GfMatrix4d> bindTransformsLS;
    skeleton.GetBindTransformsAttr().Get(&bindTransformsLS);
    TEST_REQUIRE(pxr::GfIsClose(expectedRootTransform, bindTransformsLS[0], 1e-4));
    TEST_REQUIRE(pxr::GfIsClose(expectedBoneTransform, bindTransformsLS[1], 1e-4));

    pxr::VtArray<pxr::GfMatrix4d> restTransformsLS;
    skeleton.GetRestTransformsAttr().Get(&restTransformsLS);
    TEST_REQUIRE(pxr::GfIsClose(expectedRootTransform, restTransformsLS[0], 1e-4));
    TEST_REQUIRE(pxr::GfIsClose(expectedBoneTransform, restTransformsLS[1], 1e-4));

    // Ensure the animation has been correctly rescaled
    auto animation = pxr::UsdSkelAnimation(stage->GetPrimAtPath(pxr::SdfPath("/Root/Animation")));
    TEST_REQUIRE(animation);
    {
        // Translation should be rescaled
        std::vector<double> timeSamples;
        animation.GetTranslationsAttr().GetTimeSamples(&timeSamples);

        for (auto timeSample : timeSamples) {
            pxr::VtArray<pxr::GfVec3f> translations;
            animation.GetTranslationsAttr().Get(&translations, timeSample);
            TEST_REQUIRE(translations.size() == 2);
            TEST_REQUIRE(pxr::GfIsClose(translations[1].GetLength(), 0.01f, 1e-4f));
        }
    }
    {
        // Scale should remain at 1.0
        std::vector<double> timeSamples;
        animation.GetScalesAttr().GetTimeSamples(&timeSamples);

        for (auto timeSample : timeSamples) {
            pxr::VtArray<pxr::GfVec3h> scales;
            animation.GetScalesAttr().Get(&scales, timeSample);
            TEST_REQUIRE(scales.size() == 2);
            TEST_REQUIRE(pxr::GfIsClose(scales[0], pxr::GfVec3h(1.0f, 1.0f, 1.0f), 1e-4f));
            TEST_REQUIRE(pxr::GfIsClose(scales[1], pxr::GfVec3h(1.0f, 1.0f, 1.0f), 1e-4f));
        }
    }
}

END_TEST_FIXTURE()