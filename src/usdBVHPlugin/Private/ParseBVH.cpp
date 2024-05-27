#include "ParseBVH.h"
#include "Parse.h"
#include <cstdio>
#include <fstream>
#include <iostream>

#define CHECK_GOOD(stream) if (!stream.good()) { return false; }

namespace
{
    const char * const c_WS = " \t\r\n";
    const char * const c_AlphaNumeric = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
}

namespace usdBVHPlugin
{
    bool ParseBVH(std::string const & filePath, BVHDocument & result)
    {
        std::ifstream stream(filePath, std::ios::in | std::ios::binary);
        return ParseBVH(stream, result);
    }

    Parse ParseDouble(Parse cursor, double & result)
    {
        if (!cursor)
        {
            return cursor;
        }
        unsigned int n = 0;
        auto r = sscanf_s(cursor.m_Begin, "%lf%n", &result, &n);
        if (r != 1)
        {
            return {};
        }
        return {cursor.m_Begin + n, cursor.m_End};
    }

    Parse ParseUInt(Parse cursor, unsigned int & result)
    {
        if (!cursor)
        {
            return cursor;
        }
        unsigned int n = 0;
        auto r = sscanf_s(cursor.m_Begin, "%u%n", &result, &n);
        if (r != 1)
        {
            return {};
        }
        return {cursor.m_Begin + n, cursor.m_End};
    }

    Parse ParseJointOffset(Parse cursor, BVHOffset & offset)
    {
        Parse result = cursor.String("OFFSET").Skip(c_WS);
        result = ParseDouble(result, offset.m_X).Skip(c_WS);
        result = ParseDouble(result, offset.m_Y).Skip(c_WS);
        return ParseDouble(result, offset.m_Z).Skip(c_WS);
    }

    Parse ParseJointChannels(Parse cursor, unsigned int & numChannels, uint32_t & channels)
    {
        cursor = cursor.String("CHANNELS").Skip(c_WS);
        cursor = ParseUInt(cursor, numChannels).Skip(c_WS);
        if (!cursor)
        {
            return cursor;
        }

        for (size_t i = 0; i < numChannels; ++i)
        {
            std::string token;
            cursor = cursor.Skip(c_WS).Capture(
                token,
                [&](Parse const& cursor) {
                    return cursor.AnyOf({
                        [&](Parse const& cursor) { return cursor.String("Xposition"); },
                        [&](Parse const& cursor) { return cursor.String("Yposition"); },
                        [&](Parse const& cursor) { return cursor.String("Zposition"); },
                        [&](Parse const& cursor) { return cursor.String("Xrotation"); },
                        [&](Parse const& cursor) { return cursor.String("Yrotation"); },
                        [&](Parse const& cursor) { return cursor.String("Zrotation"); }
                    });
                }
            );
            if (!cursor)
            {
                return cursor;
            }
            if (token == "Xposition") {
                channels |= static_cast<unsigned int>(BVHChannel::XPosition) << (3 * i);
            }
            else if (token == "Yposition") {
                channels |= static_cast<unsigned int>(BVHChannel::YPosition) << (3 * i);
            }
            else if (token == "Zposition") {
                channels |= static_cast<unsigned int>(BVHChannel::ZPosition) << (3 * i);
            }
            else if (token == "Xrotation") {
                channels |= static_cast<unsigned int>(BVHChannel::XRotation) << (3 * i);
            }
            else if (token == "Yrotation") {
                channels |= static_cast<unsigned int>(BVHChannel::YRotation) << (3 * i);
            }
            else if (token == "Zrotation") {
                channels |= static_cast<unsigned int>(BVHChannel::ZRotation) << (3 * i);
            }
        }
        return cursor.Skip(c_WS);
    }

    Parse ParseJointHierarchy(Parse cursor, size_t currentJointIndex, BVHDocument & document)
    {
        cursor = cursor.Char('{').Skip(c_WS);
        cursor = ParseJointOffset(cursor, document.m_JointOffsets[currentJointIndex]);
        cursor = ParseJointChannels(cursor, document.m_JointNumChannels[currentJointIndex], document.m_JointChannels[currentJointIndex]);
        cursor = cursor.AnyOf({
            [&](Parse const & cursor) {
                Parse result = cursor
                    .String("End Site")
                    .Skip(c_WS)
                    .Char('{')
                    .Skip(c_WS);

                BVHOffset offset;
                result = ParseJointOffset(result, offset);
                result = result.Char('}').Skip(c_WS);
                if (result)
                {
                    document.m_JointNames.push_back("END");
                    document.m_JointParents.push_back(static_cast<unsigned int>(currentJointIndex));
                    document.m_JointOffsets.push_back(offset);
                    document.m_JointNumChannels.push_back(0);
                    document.m_JointChannels.push_back(0);
                }
                return result;
            },
            [&](Parse const & cursor) {
                return cursor.AtLeast(0, [&](Parse const & cursor) {
                    std::string childName;
                    Parse result = cursor
                        .String("JOINT")
                        .Skip(c_WS)
                        .Capture(childName, [&](Parse const & cursor) {
                            return cursor.AtLeast(1, [](Parse const & cursor) {
                                return cursor.AnyOf(c_AlphaNumeric);
                            });
                        })
                        .Skip(c_WS);
                    ;
                    if (result)
                    {
                        document.m_JointNames.push_back(childName);
                        document.m_JointParents.push_back(static_cast<unsigned int>(currentJointIndex));
                        document.m_JointOffsets.push_back({});
                        document.m_JointNumChannels.push_back(0);
                        document.m_JointChannels.push_back(0);
                        result = ParseJointHierarchy(result, document.m_JointNames.size() - 1, document);
                    }
                    return result;
                });
            },
        });
        return cursor.Char('}').Skip(c_WS);
    }

    bool ParseBVH(std::ifstream & stream, BVHDocument & result)
    {
        CHECK_GOOD(stream);

        stream.seekg(0, std::ios_base::end);
        CHECK_GOOD(stream);
        size_t const totalSize = stream.tellg();
        CHECK_GOOD(stream);
        stream.seekg(0, std::ios_base::beg);
        CHECK_GOOD(stream);

        std::vector<char> contents;
        contents.resize(totalSize);
        stream.read(contents.data(), totalSize);
        CHECK_GOOD(stream);

        result.m_JointNames.push_back({});
        result.m_JointParents.push_back(-1);
        result.m_JointOffsets.push_back({});
        result.m_JointNumChannels.push_back(0);
        result.m_JointChannels.push_back(0);

        Parse cursor = Parse{ contents.data(), contents.data() + contents.size() }
            .String("HIERARCHY")
            .Skip(c_WS)
            .String("ROOT")
            .Skip(c_WS)
            .Capture(result.m_JointNames[0], [=](Parse const & cursor) { 
                return cursor.AtLeast(1, [=](Parse const & cursor) {
                    return cursor.AnyOf(c_AlphaNumeric);
                });
            })
            .Skip(c_WS);

        if (!cursor)
        {
            return false;
        }

        cursor = ParseJointHierarchy(cursor, 0, result);
        if (!cursor)
        {
            return false;
        }

        return true;
    }
}