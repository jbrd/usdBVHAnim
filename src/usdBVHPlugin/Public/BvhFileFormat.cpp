#include <pxr/pxr.h>
#include <pxr/base/gf/rotation.h>
#include <pxr/base/tf/declarePtrs.h>
#include <pxr/base/tf/enum.h>
#include <pxr/base/tf/registryManager.h>
#include <pxr/base/tf/staticTokens.h>
#include <pxr/base/tf/token.h>
#include <pxr/usd/sdf/data.h>
#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/sdf/fileFormat.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdSkel/bindingAPI.h>
#include <pxr/usd/usdSkel/root.h>
#include <pxr/usd/usdSkel/skeleton.h>
#include <vector>

#include "ParseBVH.h"
#include "Version.h"

PXR_NAMESPACE_USING_DIRECTIVE

namespace usdBVHPlugin
{
    class BvhFileFormat : public SdfFileFormat
    {
    protected:
        BvhFileFormat();
        virtual ~BvhFileFormat();

    public:
        bool CanRead(std::string const & filePath) const override;
        bool Read(SdfLayer * layer, std::string const & resolvedPath, bool metadataOnly) const override;
        bool WriteToString(SdfLayer const & layer, std::string * str, std::string const & comment) const override;
        bool WriteToStream(SdfSpecHandle const & spec, std::ostream & out, size_t indent) const override;

        SDF_FILE_FORMAT_FACTORY_ACCESS;
    };

    enum class BvhError
    {
        BVH_FAILED_TO_READ
    };

    TF_REGISTRY_FUNCTION(TfEnum)
    {
        TF_ADD_ENUM_NAME(BvhError::BVH_FAILED_TO_READ, "Failed to read BVH file");
    };

    TF_DECLARE_PUBLIC_TOKENS(
        BvhFileFormatTokens,
        ((Id, "bvhFileFormat"))
        ((Version, c_ProjectVersion))
        ((Target, "usd"))
        ((Extension, "bvh"))
    );

    TF_DEFINE_PUBLIC_TOKENS(
        BvhFileFormatTokens,
        ((Id, "bvhFileFormat"))
        ((Version, c_ProjectVersion))
        ((Target, "usd"))
        ((Extension, "bvh"))
    );

    BvhFileFormat::BvhFileFormat()
    :   SdfFileFormat(
            BvhFileFormatTokens->Id,
            BvhFileFormatTokens->Version,
            BvhFileFormatTokens->Target,
            BvhFileFormatTokens->Extension
        )
    {
    }

    BvhFileFormat::~BvhFileFormat()
    {
    }

    bool BvhFileFormat::CanRead(std::string const & /*filePath*/) const
    {
        return true;
    }

    bool BvhFileFormat::Read(SdfLayer * layer, std::string const & resolvedPath, bool /*metadataOnly*/) const
    {
        if (!TF_VERIFY(layer))
        {
            return false;
        }

        BVHDocument document;
        if (!ParseBVH(resolvedPath, document))
        {
            TF_ERROR(BvhError::BVH_FAILED_TO_READ, TfEnum::GetDisplayName(BvhError::BVH_FAILED_TO_READ));
            return false;
        }

        SdfLayerRefPtr skelLayer = SdfLayer::CreateAnonymous(".usda");
        UsdStageRefPtr skelStage = UsdStage::Open(skelLayer);
        UsdSkelRoot skelRoot = UsdSkelRoot::Define(skelStage, SdfPath("/SkelRoot"));
        UsdSkelSkeleton skeleton = UsdSkelSkeleton::Define(skelStage, SdfPath("/SkelRoot/Skeleton"));
        UsdAttribute jointsAttr = skeleton.CreateJointsAttr();
        UsdAttribute bindTransformsAttr = skeleton.CreateBindTransformsAttr();
        UsdAttribute restTransformsAttr = skeleton.CreateRestTransformsAttr();

        // Walk the joint hierarchy from root to leaf to calculate model-space
        // bind pose transforms from OFFSET data in the BVH
        VtArray<GfMatrix4d> bindPoseLS;
        VtArray<GfMatrix4d> bindPoseMS;
        for (size_t i = 0; i < document.m_JointNames.size(); ++i)
        {
            GfMatrix4d parentMS;
            parentMS.SetIdentity();
            if (document.m_JointParents[i] >= 0)
            {
                parentMS = bindPoseMS[document.m_JointParents[i]];
            }

            auto offset = document.m_JointOffsets[i];
            auto matrix = GfMatrix4d();
            matrix.SetTranslate(GfVec3d(offset.m_X, offset.m_Y, offset.m_Z));
            bindPoseLS.push_back(matrix);
            bindPoseMS.push_back(matrix * parentMS);
        }

        // Populate skeleton attributes
        VtArray<TfToken> jointPaths;
        for (size_t jointIndex = 0; jointIndex < document.m_JointNames.size(); ++jointIndex)
        {
            std::string jointPath = document.m_JointNames[jointIndex];
            int parentIndex = document.m_JointParents[jointIndex];
            BVHOffset currentOffset = document.m_JointOffsets[jointIndex];
            while (parentIndex >= 0)
            {
                jointPath = document.m_JointNames[parentIndex] + "/" + jointPath;
                parentIndex = document.m_JointParents[parentIndex];
            }
            jointPaths.push_back(TfToken(jointPath));
        }
        jointsAttr.Set(jointPaths);
        bindTransformsAttr.Set(bindPoseMS);
        restTransformsAttr.Set(bindPoseLS);

        // Add skel root and transfer all data to stage
        UsdSkelBindingAPI skelRootBinding(skelRoot.GetPrim());
        skelRootBinding.CreateSkeletonRel().AddTarget(SdfPath("/SkelRoot/Skeleton"));
        skelStage->SetDefaultPrim(skelRoot.GetPrim());
        layer->TransferContent(skelLayer);
        return true;
    }

    bool BvhFileFormat::WriteToString(SdfLayer const & /*layer*/, std::string * /*str*/, std::string const & /*comment*/) const
    {
        return false;
    }

    bool BvhFileFormat::WriteToStream(SdfSpecHandle const & /*spec*/, std::ostream & /*out*/, size_t /*indent*/) const
    {
        return false;
    }

    TF_DECLARE_WEAK_AND_REF_PTRS(BvhFileFormat);

    TF_REGISTRY_FUNCTION(TfType)
    {
        SDF_DEFINE_FILE_FORMAT(BvhFileFormat, SdfFileFormat);
    }
}