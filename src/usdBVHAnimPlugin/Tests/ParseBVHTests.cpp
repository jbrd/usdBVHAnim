#include "ParseBVH.h"
#include "Tests.h"
#include <cmath>
#include <sstream>

using namespace usdBVHAnimPlugin;

// See 'data/test_bvh.*' for data source files
static const char s_TestBVH[] =
    R"(HIERARCHY
ROOT Root
{
	OFFSET 0.000000 0.000000 0.000000
	CHANNELS 6 Xposition Yposition Zposition Xrotation Yrotation Zrotation
	JOINT Foo
	{
		OFFSET 0.000000 0.000000 1.000000
		CHANNELS 3 Xrotation Yrotation Zrotation
		End Site
		{
			OFFSET 0.000000 0.000000 1.000000
		}
	}
}
MOTION
Frames: 20
Frame Time: 0.041667
0.000000 0.000000 0.000000 -0.000000 0.000000 -0.000000 -0.000000 0.000000 -0.000000 
0.000000 0.008019 0.000000 0.721680 0.000000 -0.000000 -0.000000 0.721680 -0.000000 
0.000000 0.030908 0.000000 2.781747 0.000000 -0.000000 -0.000000 2.781747 -0.000000 
0.000000 0.066919 0.000000 6.022745 0.000000 -0.000000 0.000000 6.022745 -0.000000 
0.000000 0.114302 0.000000 10.287214 0.000000 -0.000000 -0.000000 10.287214 -0.000000 
0.000000 0.171308 0.000000 15.417699 0.000000 -0.000000 -0.000001 15.417699 -0.000000 
0.000000 0.236186 0.000000 21.256744 0.000000 -0.000000 -0.000000 21.256744 -0.000000 
0.000000 0.307188 0.000000 27.646890 0.000000 -0.000000 -0.000000 27.646890 -0.000000 
0.000000 0.382563 0.000000 34.430676 0.000000 -0.000000 -0.000000 34.430676 -0.000000 
0.000000 0.460563 0.000000 41.450656 0.000000 -0.000000 -0.000000 41.450656 -0.000000 
0.000000 0.539437 0.000000 48.549353 0.000000 -0.000000 -0.000000 48.549353 -0.000000 
0.000000 0.617437 0.000000 55.569337 0.000000 -0.000000 -0.000000 55.569333 -0.000000 
0.000000 0.692812 0.000000 62.353125 0.000000 -0.000000 0.000002 62.353125 -0.000000 
0.000000 0.763814 0.000000 68.743259 0.000000 -0.000000 -0.000001 68.743259 -0.000000 
0.000000 0.828692 0.000000 74.582312 0.000000 -0.000000 0.000002 74.582312 -0.000000 
0.000000 0.885698 0.000000 79.712788 0.000000 -0.000000 -0.000000 79.712788 -0.000000 
0.000000 0.933081 0.000000 83.977258 0.000000 -0.000000 0.000001 83.977258 -0.000000 
0.000000 0.969092 0.000000 87.218251 0.000000 -0.000000 -0.000000 87.218251 -0.000000 
0.000000 0.991981 0.000000 89.278345 0.000000 -0.000000 -0.000000 89.278345 -0.000000 
0.000000 1.000000 0.000000 90.000003 0.000000 -0.000000 -0.000000 89.999996 0.000000)";

BEGIN_TEST_FIXTURE(ParseBVHTests)

TEST(ParseBVH_ParseTest)
{
    float constexpr c_Tolerance = 1e-6f;

    // Parse the BVH data and ensure it is successful
    std::istringstream stream(s_TestBVH, std::ios::in | std::ios::binary);
    usdBVHAnimPlugin::BVHDocument document;
    bool result = usdBVHAnimPlugin::ParseBVH(stream, document);
    TEST_REQUIRE(result == true);

    // Validate the parsed document
    size_t constexpr c_NumJoints = 2;
    size_t constexpr c_NumFrames = 20;
    TEST_REQUIRE(document.m_JointNames.size() == c_NumJoints);
    TEST_REQUIRE(document.m_JointNames[0] == "Root");
    TEST_REQUIRE(document.m_JointNames[1] == "Foo");
    TEST_REQUIRE(document.m_JointParents.size() == c_NumJoints);
    TEST_REQUIRE(document.m_JointParents[0] == -1);
    TEST_REQUIRE(document.m_JointParents[1] == 0);
    TEST_REQUIRE(document.m_JointOffsets.size() == c_NumJoints);
    TEST_REQUIRE(std::fabs(document.m_JointOffsets[0].m_Translation[0] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_JointOffsets[0].m_Translation[1] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_JointOffsets[0].m_Translation[2] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_JointOffsets[1].m_Translation[0] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_JointOffsets[1].m_Translation[1] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_JointOffsets[1].m_Translation[2] - 1.0) < c_Tolerance);
    TEST_REQUIRE(document.m_JointNumChannels.size() == c_NumJoints);
    TEST_REQUIRE(document.m_JointNumChannels[0] == 6);
    TEST_REQUIRE(document.m_JointNumChannels[1] == 3);
    TEST_REQUIRE(document.m_JointChannels.size() == c_NumJoints);
    TEST_REQUIRE(((document.m_JointChannels[0] >> 0) & BVHChannel::BitMask) == BVHChannel::XPosition);
    TEST_REQUIRE(((document.m_JointChannels[0] >> 3) & BVHChannel::BitMask) == BVHChannel::YPosition);
    TEST_REQUIRE(((document.m_JointChannels[0] >> 6) & BVHChannel::BitMask) == BVHChannel::ZPosition);
    TEST_REQUIRE(((document.m_JointChannels[0] >> 9) & BVHChannel::BitMask) == BVHChannel::XRotation);
    TEST_REQUIRE(((document.m_JointChannels[0] >> 12) & BVHChannel::BitMask) == BVHChannel::YRotation);
    TEST_REQUIRE(((document.m_JointChannels[0] >> 15) & BVHChannel::BitMask) == BVHChannel::ZRotation);
    TEST_REQUIRE(((document.m_JointChannels[0] >> 18) & BVHChannel::BitMask) == BVHChannel::None);
    TEST_REQUIRE(((document.m_JointChannels[1] >> 0) & BVHChannel::BitMask) == BVHChannel::XRotation);
    TEST_REQUIRE(((document.m_JointChannels[1] >> 3) & BVHChannel::BitMask) == BVHChannel::YRotation);
    TEST_REQUIRE(((document.m_JointChannels[1] >> 6) & BVHChannel::BitMask) == BVHChannel::ZRotation);
    TEST_REQUIRE(((document.m_JointChannels[1] >> 9) & BVHChannel::BitMask) == BVHChannel::None);
    TEST_REQUIRE(std::fabs(document.m_FrameTime - 0.041667) < c_Tolerance);
    TEST_REQUIRE(document.m_FrameTransforms.size() == c_NumFrames * c_NumJoints);

    // Validate first frame bone transforms
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[0].m_Translation[0] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[0].m_Translation[1] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[0].m_Translation[2] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[0].m_RotationQuat[0] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[0].m_RotationQuat[1] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[0].m_RotationQuat[2] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[0].m_RotationQuat[3] - 1.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[1].m_Translation[0] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[1].m_Translation[1] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[1].m_Translation[2] - 1.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[1].m_RotationQuat[0] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[1].m_RotationQuat[1] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[1].m_RotationQuat[2] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[1].m_RotationQuat[3] - 1.0) < c_Tolerance);

    // Validate last frame bone transforms
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[19 * 2 + 0].m_Translation[0] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[19 * 2 + 0].m_Translation[1] - 1.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[19 * 2 + 0].m_Translation[2] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[19 * 2 + 0].m_RotationQuat[0] - std::sin(M_PI * 0.25f)) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[19 * 2 + 0].m_RotationQuat[1] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[19 * 2 + 0].m_RotationQuat[2] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[19 * 2 + 0].m_RotationQuat[3] - std::cos(M_PI * 0.25f)) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[19 * 2 + 1].m_Translation[0] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[19 * 2 + 1].m_Translation[1] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[19 * 2 + 1].m_Translation[2] - 1.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[19 * 2 + 1].m_RotationQuat[0] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[19 * 2 + 1].m_RotationQuat[1] - std::sin(M_PI * 0.25f)) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[19 * 2 + 1].m_RotationQuat[2] - 0.0) < c_Tolerance);
    TEST_REQUIRE(std::fabs(document.m_FrameTransforms[19 * 2 + 1].m_RotationQuat[3] - std::cos(M_PI * 0.25f)) < c_Tolerance);
}

END_TEST_FIXTURE()