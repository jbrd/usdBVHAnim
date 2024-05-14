#include <pxr/pxr.h>
#include <pxr/base/tf/declarePtrs.h>
#include <pxr/base/tf/registryManager.h>
#include <pxr/base/tf/staticTokens.h>
#include <pxr/base/tf/token.h>
#include <pxr/usd/sdf/fileFormat.h>
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

    bool BvhFileFormat::Read(SdfLayer * /*layer*/, std::string const & resolvedPath, bool /*metadataOnly*/) const
    {
        ParseBVH(resolvedPath);
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