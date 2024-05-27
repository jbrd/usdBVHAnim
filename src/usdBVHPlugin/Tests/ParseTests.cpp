#include "Parse.h"
#include "Tests.h"

using namespace usdBVHPlugin;

BEGIN_TEST_FIXTURE(ParseTests)

    TEST(Parse_IsNotValid_By_Default)
    {
        Parse parse;
        TEST_REQUIRE(parse.m_Begin == nullptr);
        TEST_REQUIRE(parse.m_End == nullptr);
        TEST_REQUIRE(!parse);
    }

    TEST(Parse_IsValid_Before_EndOfStream)
    {
        char stream = 'x';
        Parse parse = {&stream, &stream + 1};
        TEST_REQUIRE(parse);
    }

    TEST(Parse_IsValid_When_EndOfStream)
    {
        char const stream = 'x';
        Parse parse = {&stream, &stream};
        TEST_REQUIRE(parse);
    }

    TEST(Parse_NotValid_When_PastEndOfStream)
    {
        char const * stream = "xy";
        Parse parse = {&stream[1], &stream[0]};
        TEST_REQUIRE(!parse);
    }

    TEST(Parse_NotValid_When_StartPointer_IsNull)
    {
        char stream = 'x';
        Parse parse = {nullptr, &stream};
        TEST_REQUIRE(!parse);
    }

    TEST(Parse_NotValid_When_EndPointer_IsNull)
    {
        char stream = 'x';
        Parse parse = {&stream, nullptr};
        TEST_REQUIRE(!parse);
    }

    TEST(Parse_Char_Valid_On_SuccessfulMatch)
    {
        char stream = 'x';
        Parse parse = {&stream, &stream + 1};
        Parse result = parse.Char('x');
        TEST_REQUIRE(result);
    }

    TEST(Parse_Char_NotValid_On_FailedMatch)
    {
        char stream = 'x';
        Parse parse = {&stream, &stream + 1};
        TEST_REQUIRE(!parse.Char('y'));
    }

    TEST(Parse_Char_NotValid_On_EndOfStream)
    {
        char stream = 'x';
        Parse parse = {&stream, &stream};
        TEST_REQUIRE(!parse.Char('x'));
    }

    TEST(Parse_Char_NotValid_On_InvalidCursor)
    {
        Parse parse;
        TEST_REQUIRE(!parse.Char('x'));
    }

    TEST(Parse_String_Valid_On_SuccessfulMatch)
    {
        char const * stream = "ABC";
        Parse parse = {stream, stream + 3};
        Parse result = parse.String("ABC");
        TEST_REQUIRE(result);
    }

    TEST(Parse_String_Valid_And_Identical_On_EmptyString)
    {
        char const * stream = "ABC";
        Parse parse = {stream, stream + 3};
        Parse result = parse.String("");
        TEST_REQUIRE(result.m_Begin == parse.m_Begin);
        TEST_REQUIRE(result.m_End == parse.m_End);
    }

    TEST(Parse_String_NotValid_On_NullString)
    {
        char const * stream = "ABC";
        Parse parse = {stream, stream + 3};
        TEST_REQUIRE(!parse.String(nullptr));
    }

    TEST(Parse_String_NotValid_On_FailedMatch)
    {
        char const * stream = "ABC";
        Parse parse = {stream, stream + 3};
        TEST_REQUIRE(!parse.String("XBC"));
        TEST_REQUIRE(!parse.String("XXC"));
        TEST_REQUIRE(!parse.String("XXX"));
    }

    TEST(Parse_String_NotValid_On_TooManyCharacters)
    {
        char const * stream = "ABC";
        Parse parse = {stream, stream + 3};
        TEST_REQUIRE(!parse.String("ABCDEFG"));
    }

    TEST(Parse_String_NotValid_On_EndOfStream)
    {
        char stream = 'x';
        Parse parse = {&stream, &stream};
        TEST_REQUIRE(!parse.String("x"));
    }

    TEST(Parse_String_NotValid_On_InvalidCursor)
    {
        Parse parse;
        TEST_REQUIRE(!parse.String("x"));
    }

    TEST(Parse_AnyOf_Char_Valid_On_FirstMatch)
    {
        char const * stream = "ABC";
        Parse parse = {stream, stream + 3};
        TEST_REQUIRE(parse.AnyOf("AX"));
    }

    TEST(Parse_AnyOf_Char_Valid_On_SecondMatch)
    {
        char const * stream = "ABC";
        Parse parse = {stream, stream + 3};
        TEST_REQUIRE("XA");
    }

    TEST(Parse_AnyOf_Char_NotValid_On_NoMatch)
    {
        char const * stream = "ABC";
        Parse parse = {stream, stream + 3};
        TEST_REQUIRE(!parse.AnyOf("XY"));
    }

    TEST(Parse_AnyOf_Char_NotValid_On_EmptyMatches)
    {
        char const * stream = "ABC";
        Parse parse = {stream, stream + 3};
        TEST_REQUIRE(!parse.AnyOf(""));
    }

    TEST(Parse_AnyOf_Char_NotValid_On_EndOfStream)
    {
        char const stream = 'x';
        Parse parse = {&stream, &stream};
        TEST_REQUIRE(!parse.AnyOf("x"));
    }

    TEST(Parse_AnyOf_Char_NotValid_On_InvalidCursor)
    {
        Parse parse;
        TEST_REQUIRE(!parse.AnyOf("XY"));
    }

    TEST(Parse_AnyOf_Char_NotValid_On_NullChars)
    {
        Parse parse;
        TEST_REQUIRE(!parse.AnyOf(nullptr));
    }

    TEST(Parse_AnyOf_Valid_On_FirstMatch)
    {
        char const * stream = "ABC";
        Parse parse = {stream, stream + 3};
        TEST_REQUIRE(parse.AnyOf({
            [](Parse const & cursor) { return cursor.Char('A'); },
            [](Parse const & cursor) { return cursor.Char('X'); }
        }));
    }

    TEST(Parse_AnyOf_Valid_On_SecondMatch)
    {
        char const * stream = "ABC";
        Parse parse = {stream, stream + 3};
        TEST_REQUIRE(parse.AnyOf({
            [](Parse const & cursor) { return cursor.Char('X'); },
            [](Parse const & cursor) { return cursor.Char('A'); }
        }));
    }

    TEST(Parse_AnyOf_NotValid_On_NoMatch)
    {
        char const * stream = "ABC";
        Parse parse = {stream, stream + 3};
        TEST_REQUIRE(!parse.AnyOf({
            [](Parse const & cursor) { return cursor.Char('X'); },
            [](Parse const & cursor) { return cursor.Char('Y'); }
        }));
    }

    TEST(Parse_AnyOf_NotValid_On_EmptyMatches)
    {
        char const * stream = "ABC";
        Parse parse = {stream, stream + 3};
        TEST_REQUIRE(!parse.AnyOf({
        }));
    }

    TEST(Parse_AnyOf_NotValid_On_EndOfStream)
    {
        char const stream = 'x';
        Parse parse = {&stream, &stream};
        TEST_REQUIRE(!parse.AnyOf({
            [](Parse const & cursor) { return cursor.Char('x'); },
        }));
    }

    TEST(Parse_AnyOf_NotValid_On_InvalidCursor)
    {
        Parse parse;
        TEST_REQUIRE(!parse.AnyOf({
            [](Parse const & cursor) { return cursor.Char('X'); },
            [](Parse const & cursor) { return cursor.Char('Y'); }
        }));
    }

    TEST(Parse_AtLeast_0_Valid_On_NoMatches)
    {
        char const * stream = "ABC";
        Parse parse = {stream, stream + 3};
        Parse result = parse.AtLeast(0, [](Parse const & cursor) { return cursor.Char('x'); });
        TEST_REQUIRE(result);
        TEST_REQUIRE(result.Char('A'));
    }

    TEST(Parse_AtLeast_0_Valid_On_OneMatch)
    {
        char const * stream = "abc";
        Parse parse = {stream, stream + 3};
        Parse result = parse.AtLeast(0, [](Parse const & cursor) { return cursor.Char('a'); });
        TEST_REQUIRE(result);
        TEST_REQUIRE(result.Char('b'));
    }

    TEST(Parse_AtLeast_0_Valid_On_SeveralMatches)
    {
        char const * stream = "aaaabc";
        Parse parse = {stream, stream + 6};
        Parse result = parse.AtLeast(0, [](Parse const & cursor) { return cursor.Char('a'); });
        TEST_REQUIRE(result);
        TEST_REQUIRE(result.Char('b'));
    }

    TEST(Parse_AtLeast_0_Valid_On_EndOfStream)
    {
        char const stream = 'x';
        Parse parse = {&stream, &stream};
        Parse result = parse.AtLeast(0, {
            [](Parse const & cursor) { return cursor.Char('x'); }
        });
        TEST_REQUIRE(result);
        TEST_REQUIRE(parse == result);
    }

    TEST(Parse_AtLeast_0_NotValid_On_InvalidCursor)
    {
        Parse parse = {};
        TEST_REQUIRE(!parse.AtLeast(1, [](Parse const & cursor) { return cursor.Char('x'); }));
    }

    TEST(Parse_AtLeast_1_NotValid_On_NoMatches)
    {
        char const * stream = "ABC";
        Parse parse = {stream, stream + 3};
        TEST_REQUIRE(!parse.AtLeast(1, [](Parse const & cursor) { return cursor.Char('x'); }));
    }

    TEST(Parse_AtLeast_1_Valid_On_OneMatch)
    {
        char const * stream = "ABC";
        Parse parse = {stream, stream + 3};
        Parse result = parse.AtLeast(1, [](Parse const & cursor) { return cursor.Char('A'); });
        TEST_REQUIRE(result);
        TEST_REQUIRE(result.Char('B'));
    }

    TEST(Parse_AtLeast_1_Valid_On_SeveralMatches)
    {
        char const * stream = "AAAABC";
        Parse parse = {stream, stream + 6};
        Parse result = parse.AtLeast(1, [](Parse const & cursor) { return cursor.Char('A'); });
        TEST_REQUIRE(result);
        TEST_REQUIRE(result.Char('B'));
    }

    TEST(Parse_AtLeast_1_NotValid_On_EndOfStream)
    {
        char const stream = 'x';
        Parse parse = {&stream, &stream};
        Parse result = parse.AtLeast(1, {
            [](Parse const & cursor) { return cursor.Char('x'); }
        });
        TEST_REQUIRE(!result);
    }

    TEST(Parse_AtLeast_1_NotValid_On_InvalidCursor)
    {
        Parse parse = {};
        TEST_REQUIRE(!parse.AtLeast(1, [](Parse const & cursor) { return cursor.Char('x'); }));
    }

    TEST(Parse_AtLeast_2_NotValid_On_NoMatches)
    {
        char const * stream = "ABC";
        Parse parse = {stream, stream + 3};
        TEST_REQUIRE(!parse.AtLeast(2, [](Parse const & cursor) { return cursor.Char('x'); }));
    }

    TEST(Parse_AtLeast_2_NotValid_On_OneMatch)
    {
        char const * stream = "ABC";
        Parse parse = {stream, stream + 3};
        TEST_REQUIRE(!parse.AtLeast(2, [](Parse const & cursor) { return cursor.Char('A'); }));
    }

    TEST(Parse_AtLeast_2_Valid_On_TwoMatches)
    {
        char const * stream = "AABC";
        Parse parse = {stream, stream + 4};
        Parse result = parse.AtLeast(2, [](Parse const & cursor) { return cursor.Char('A'); });
        TEST_REQUIRE(result);
        TEST_REQUIRE(result.Char('B'));
    }

    TEST(Parse_AtLeast_2_Valid_On_SeveralMatches)
    {
        char const * stream = "AAAABC";
        Parse parse = {stream, stream + 6};
        Parse result = parse.AtLeast(1, [](Parse const & cursor) { return cursor.Char('A'); });
        TEST_REQUIRE(result);
        TEST_REQUIRE(result.Char('B'));
    }

    TEST(Parse_AtLeast_2_NotValid_On_EndOfStream)
    {
        char const * stream = "xxx";
        Parse parse = {stream, stream};
        Parse result = parse.AtLeast(2, {
            [](Parse const & cursor) { return cursor.Char('x'); }
        });
        TEST_REQUIRE(!result);
    }

    TEST(Parse_AtLeast_2_NotValid_On_InvalidCursor)
    {
        Parse parse = {};
        TEST_REQUIRE(!parse.AtLeast(2, [](Parse const & cursor) { return cursor.Char('x'); }));
    }

    TEST(Parse_Capture_Valid_When_Match_1_Characters)
    {
        char const * stream = "xyz";
        Parse parse = {stream, stream + 3};
        std::string capture;
        Parse result = parse.Capture(capture, [](Parse const & cursor) { return cursor.String("x"); });
        TEST_REQUIRE(result);
        TEST_REQUIRE(result.String("yz"));
        TEST_REQUIRE(capture == "x");
    }

    TEST(Parse_Capture_Valid_When_Match_2_Characters)
    {
        char const * stream = "xyz";
        Parse parse = {stream, stream + 3};
        std::string capture;
        Parse result = parse.Capture(capture, [](Parse const & cursor) { return cursor.String("xy"); });
        TEST_REQUIRE(result);
        TEST_REQUIRE(result.Char('z'));
        TEST_REQUIRE(capture == "xy");
    }

    TEST(Parse_Capture_Valid_When_Match_3_Characters)
    {
        char const * stream = "xyz";
        Parse parse = {stream, stream + 3};
        std::string capture;
        Parse result = parse.Capture(capture, [](Parse const & cursor) { return cursor.String("xyz"); });
        TEST_REQUIRE(result);
        TEST_REQUIRE(capture == "xyz");
    }

    TEST(Parse_Capture_Fail_When_Function_Fails)
    {
        char const * stream = "xyz";
        Parse parse = {stream, stream + 3};
        std::string capture;
        Parse result = parse.Capture(capture, [](Parse const & cursor) { return cursor.String("xyz123"); });
        TEST_REQUIRE(!result);
    }

    TEST(Parse_Capture_Fail_On_EndOfStream)
    {
        char const * stream = "xyz";
        Parse parse = {stream, stream};
        std::string capture;
        Parse result = parse.Capture(capture, [](Parse const & cursor) { return cursor.Char('x'); });
        TEST_REQUIRE(!result);
    }

    TEST(Parse_Capture_Fail_On_InvalidCursor)
    {
        Parse parse;
        std::string capture;
        Parse result = parse.Capture(capture, [](Parse const & cursor) { return cursor.Char('x'); });
        TEST_REQUIRE(!result);
    }

END_TEST_FIXTURE()