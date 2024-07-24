#pragma once
#include <string>
#include <vector>

namespace usdBVHPlugin {
enum class BVHChannel {
    None = 0,
    XPosition,
    YPosition,
    ZPosition,
    XRotation,
    YRotation,
    ZRotation,
    Mask = 0b111
};

inline BVHChannel operator&(uint32_t value, BVHChannel mask)
{
    return static_cast<BVHChannel>(static_cast<uint32_t>(value) & static_cast<uint32_t>(mask));
}

struct BVHOffset {
    double m_Translation[3];
};

struct BVHTransform {
    double m_RotationQuat[4];
    double m_Translation[3];
};

struct BVHDocument {
    std::vector<std::string> m_JointNames;
    std::vector<int> m_JointParents;
    std::vector<BVHOffset> m_JointOffsets;
    std::vector<unsigned int> m_JointNumChannels;
    std::vector<uint32_t> m_JointChannels;
    double m_FrameTime;
    std::vector<BVHTransform> m_FrameTransforms;
};

bool ParseBVH(std::string const& filePath, BVHDocument& result);
bool ParseBVH(std::istream& stream, BVHDocument& result);
} // namespace usdBVHPlugin