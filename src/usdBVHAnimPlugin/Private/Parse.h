#pragma once
#include <functional>
#include <string>

namespace usdBVHAnimPlugin {
//! A small class for parsing strings.
struct Parse {
    char const* m_Begin = nullptr;
    char const* m_End = nullptr;

    //! Returns `true` if the Parse object is still valid, or `false` otherwise.
    //! This method can be used to determine whether an error has occurred during
    //! parsing (if it has, an invalid `Parse` object would be returned).
    //! A `Parse` object is considered valid if it still refers to a string. This
    //! includes referring to the end of the string (since, its perfectly valid for
    //! a parse function to succeed and reach the end of the string).
    operator bool() const
    {
        return m_Begin && m_End && m_Begin <= m_End;
    }

    //! Parse a single character from the string. If that character is matched,
    //! a new `Parse` object is returned beginning at the next character. Otherwise,
    //! an invalid `Parse` object is returned.
    Parse Char(char value) const
    {
        if ((*this) && *m_Begin == value) {
            return Parse { m_Begin + 1, m_End };
        }
        return Parse {};
    }

    Parse String(const char* str) const
    {
        if (!str) {
            return {};
        }
        Parse result = *this;
        while (str && *str != '\0') {
            result = result.Char(*str);
            ++str;
        }
        return result;
    }

    Parse AnyOf(const char* chars) const
    {
        while (chars && *chars) {
            auto result = Char(*chars);
            if (result) {
                return result;
            }
            ++chars;
        }
        return {};
    }

    Parse AnyOf(std::initializer_list<std::function<Parse(Parse const&)>> const& args) const
    {
        for (auto const& item : args) {
            auto result = item(*this);
            if (result) {
                return result;
            }
        }
        return {};
    }

    Parse AtLeast(size_t n, std::function<Parse(Parse)> const& function) const
    {
        Parse cursor = *this;
        while (cursor && n > 0) {
            cursor = function(cursor);
            --n;
        }
        if (!cursor || n > 0) {
            return {};
        }
        while (cursor) {
            Parse next = function(cursor);
            if (next) {
                cursor = next;
            } else {
                break;
            }
        }
        return cursor;
    }

    Parse Skip(char const* chars) const
    {
        return AtLeast(0, [=](Parse const& cursor) { return cursor.AnyOf(chars); });
    }

    Parse Capture(std::string& result, std::function<Parse(Parse)> const& function) const
    {
        Parse next = function(*this);
        if (next) {
            result = std::string(m_Begin, next.m_Begin - m_Begin);
            return next;
        } else {
            return {};
        }
    }
};
} // namespace usdBVHAnimPlugin