#include <pxr/base/gf/matrix4f.h>
#include <pxr/base/gf/rotation.h>
#include <pxr/base/tf/declarePtrs.h>
#include <pxr/base/tf/enum.h>
#include <pxr/base/tf/registryManager.h>
#include <pxr/base/tf/staticTokens.h>
#include <pxr/base/tf/token.h>
#include <pxr/pxr.h>
#include <pxr/usd/sdf/data.h>
#include <pxr/usd/sdf/fileFormat.h>
#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/boundable.h>
#include <pxr/usd/usdSkel/animation.h>
#include <pxr/usd/usdSkel/bindingAPI.h>
#include <pxr/usd/usdSkel/root.h>
#include <pxr/usd/usdSkel/skeleton.h>
#include <vector>

#include "ParseBVH.h"
#include "Version.h"

using namespace usdBVHAnimPlugin;

PXR_NAMESPACE_OPEN_SCOPE

//! An SdfFileFormat for BVH animation data, transcoding the BVH data into usdSkel prims.
class BvhFileFormat : public SdfFileFormat {
protected:
    BvhFileFormat();
    virtual ~BvhFileFormat() = default;

public:
    //! Returns `true` if the given file path can be read by this plug-in or `false` otherwise.
    bool CanRead(std::string const& filePath) const override;

    //! Reads the given BVH file into the given SdfLayer. Returns `true` on success or `false` on failure.
    bool Read(SdfLayer* layer, std::string const& resolvedPath, bool metadataOnly) const override;

    //! This function always returns false. Only reading of BVH files is supported.
    bool WriteToString(SdfLayer const& layer, std::string* str, std::string const& comment) const override
    {
        // These are unused, but names kept so that they show up in docs
        TF_UNUSED(layer);
        TF_UNUSED(str);
        TF_UNUSED(comment);
        return false;
    }

    //! This function always returns false. Only reading of BVH files is supported.
    bool WriteToStream(SdfSpecHandle const& spec, std::ostream& out, size_t indent) const override
    {
        // These are unused, but names kept so that they show up in docs
        TF_UNUSED(spec);
        TF_UNUSED(out);
        TF_UNUSED(indent);
        return false;
    }

    SDF_FILE_FORMAT_FACTORY_ACCESS;
};

enum class BvhError {
    BVH_FAILED_TO_READ
};

TF_REGISTRY_FUNCTION(TfEnum)
{
    TF_ADD_ENUM_NAME(BvhError::BVH_FAILED_TO_READ, "Failed to read BVH file");
};

TF_DECLARE_PUBLIC_TOKENS(
    BvhFileFormatTokens,
    ((Id, "bvhFileFormat"))((Version, c_ProjectVersion))((Target, "usd"))((Extension, "bvh")));

TF_DEFINE_PUBLIC_TOKENS(
    BvhFileFormatTokens,
    ((Id, "bvhFileFormat"))((Version, c_ProjectVersion))((Target, "usd"))((Extension, "bvh")));

BvhFileFormat::BvhFileFormat()
    : SdfFileFormat(
          BvhFileFormatTokens->Id,
          BvhFileFormatTokens->Version,
          BvhFileFormatTokens->Target,
          BvhFileFormatTokens->Extension)
{
}

bool BvhFileFormat::CanRead(std::string const& /*filePath*/) const
{
    return true;
}

bool BvhFileFormat::Read(SdfLayer* layer, std::string const& resolvedPath, bool /*metadataOnly*/) const
{
    if (!TF_VERIFY(layer)) {
        return false;
    }

    BVHDocument document;
    if (!ParseBVH(resolvedPath, document)) {
        TF_ERROR(BvhError::BVH_FAILED_TO_READ, TfEnum::GetDisplayName(BvhError::BVH_FAILED_TO_READ));
        return false;
    }

    SdfLayerRefPtr skelLayer = SdfLayer::CreateAnonymous(".usda");
    UsdStageRefPtr skelStage = UsdStage::Open(skelLayer);
    UsdSkelRoot skelRoot = UsdSkelRoot::Define(skelStage, SdfPath("/Root"));
    UsdSkelSkeleton skeleton = UsdSkelSkeleton::Define(skelStage, SdfPath("/Root/Skeleton"));
    UsdAttribute jointsAttr = skeleton.CreateJointsAttr();
    UsdAttribute bindTransformsAttr = skeleton.CreateBindTransformsAttr();
    UsdAttribute restTransformsAttr = skeleton.CreateRestTransformsAttr();

    // Walk the joint hierarchy from root to leaf to calculate model-space
    // bind pose transforms from OFFSET data in the BVH
    VtArray<GfMatrix4d> bindPoseLS;
    VtArray<GfMatrix4d> bindPoseMS;
    for (size_t i = 0; i < document.m_JointNames.size(); ++i) {
        GfMatrix4d parentMS;
        parentMS.SetIdentity();
        if (document.m_JointParents[i] >= 0) {
            parentMS = bindPoseMS[document.m_JointParents[i]];
        }

        auto offset = document.m_JointOffsets[i];
        auto matrix = GfMatrix4d();
        matrix.SetTranslate(GfVec3d(offset.m_Translation[0], offset.m_Translation[1], offset.m_Translation[2]));

        bindPoseLS.push_back(matrix);
        bindPoseMS.push_back(matrix * parentMS);
    }

    // Populate skeleton attributes
    VtArray<TfToken> jointPaths;
    for (size_t jointIndex = 0; jointIndex < document.m_JointNames.size(); ++jointIndex) {
        std::string jointPath = document.m_JointNames[jointIndex];
        int parentIndex = document.m_JointParents[jointIndex];
        while (parentIndex != BVHDocument::c_RootParentIndex) {
            jointPath = document.m_JointNames[parentIndex] + "/" + jointPath;
            parentIndex = document.m_JointParents[parentIndex];
        }
        jointPaths.push_back(TfToken(jointPath));
    }
    jointsAttr.Set(jointPaths);
    bindTransformsAttr.Set(bindPoseMS);
    restTransformsAttr.Set(bindPoseLS);

    // Add animation data
    UsdSkelAnimation animation = UsdSkelAnimation::Define(skelStage, SdfPath("/Root/Animation"));
    UsdAttribute animJointsAttr = animation.CreateJointsAttr();
    UsdAttribute animTranslationsAttr = animation.CreateTranslationsAttr();
    UsdAttribute animRotationsAttr = animation.CreateRotationsAttr();
    UsdAttribute animScalesAttr = animation.CreateScalesAttr();

    size_t numFrames = document.m_FrameTransforms.size() / document.m_JointNames.size();
    double framesPerSecond = 1.0 / document.m_FrameTime;
    skelLayer->SetTimeCodesPerSecond(framesPerSecond);
    skelLayer->SetStartTimeCode(1.0);
    skelLayer->SetEndTimeCode(static_cast<double>(1 + numFrames));

    UsdGeomBoundable boundable(skelRoot.GetPrim());
    UsdAttribute extents = boundable.CreateExtentAttr();

    for (size_t frameIndex = 0; frameIndex < numFrames; ++frameIndex) {
        VtArray<GfVec3f> animTranslations;
        VtArray<GfQuatf> animRotations;

        for (size_t jointIndex = 0; jointIndex < document.m_JointNames.size(); ++jointIndex) {
            size_t transformIndex = frameIndex * document.m_JointNames.size() + jointIndex;

            auto const& frame = document.m_FrameTransforms[transformIndex];
            GfRotation frameRotation = GfQuatd(frame.m_RotationQuat[3], frame.m_RotationQuat[0], frame.m_RotationQuat[1], frame.m_RotationQuat[2]);
            auto localTransform = GfMatrix4f();
            localTransform.SetTransform(frameRotation, GfVec3f(static_cast<float>(frame.m_Translation[0]), static_cast<float>(frame.m_Translation[1]), static_cast<float>(frame.m_Translation[2])));
            animTranslations.push_back(localTransform.ExtractTranslation());
            animRotations.push_back(localTransform.ExtractRotationQuat());
        }

        skelLayer->SetTimeSample(animTranslationsAttr.GetPath(), 1.0 + frameIndex, animTranslations);
        skelLayer->SetTimeSample(animRotationsAttr.GetPath(), 1.0 + frameIndex, animRotations);
    }

    VtArray<GfVec3h> animScales;
    for (size_t jointIndex = 0; jointIndex < document.m_JointNames.size(); ++jointIndex) {
        animScales.push_back(GfVec3h(1.0f, 1.0f, 1.0f));
    }
    skelLayer->SetTimeSample(animScalesAttr.GetPath(), 1.0, animScales);

    animJointsAttr.Set(jointPaths);

    UsdSkelBindingAPI::Apply(skelRoot.GetPrim());
    UsdSkelBindingAPI::Apply(skeleton.GetPrim());
    UsdSkelBindingAPI skelRootBinding(skelRoot.GetPrim());
    skelRootBinding.CreateSkeletonRel().AddTarget(SdfPath("/Root/Skeleton"));
    UsdSkelBindingAPI skelBinding(skeleton.GetPrim());
    skelBinding.CreateAnimationSourceRel().AddTarget(SdfPath("/Root/Animation"));

    // Calculate extents
    for (size_t frameIndex = 0; frameIndex < numFrames; ++frameIndex) {
        VtVec3fArray extent;
        UsdGeomBoundable::ComputeExtentFromPlugins(skelRoot, 1.0 + frameIndex, &extent);
        skelLayer->SetTimeSample(extents.GetPath(), 1.0 + frameIndex, extent);
    }

    // Add skel root and transfer all data to stage
    skelStage->SetDefaultPrim(skelRoot.GetPrim());
    layer->TransferContent(skelLayer);
    return true;
}

TF_DECLARE_WEAK_AND_REF_PTRS(BvhFileFormat);

TF_REGISTRY_FUNCTION(TfType)
{
    SDF_DEFINE_FILE_FORMAT(BvhFileFormat, SdfFileFormat);
}PXR_NAMESPACE_CLOSE_SCOPE