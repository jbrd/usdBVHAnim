#pragma once
#include <string>
#include <vector>

namespace usdBVHPlugin
{
    enum class BVHChannel
    {
        XPosition,
        YPosition,
        ZPosition,
        XRotation,
        YRotation,
        ZRotation
    };

    struct BVHOffset
    {
        double m_X;
        double m_Y;
        double m_Z;
    };

    struct BVHDocument
    {
        std::vector<std::string> m_JointNames;
        std::vector<int> m_JointParents;
        std::vector<BVHOffset> m_JointOffsets;
        std::vector<unsigned int> m_JointNumChannels;
        std::vector<uint32_t> m_JointChannels;
    };

    bool ParseBVH(std::string const & filePath, BVHDocument & result);
    bool ParseBVH(std::ifstream & stream, BVHDocument & result);
}