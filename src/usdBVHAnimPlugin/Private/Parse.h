#pragma once
#include <functional>
#include <string>

namespace usdBVHPlugin {
struct Parse {
    char const* m_Begin = nullptr;
    char const* m_End = nullptr;

    operator bool() const
    {
        return m_Begin && m_End && m_Begin <= m_End;
    }

    Parse Char(char value) const
    {
        if ((*this) && *m_Begin == value) {
            return Parse{ m_Begin + 1, m_End };
        }
        return Parse{};
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
            }
            else {
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
        }
        else {
            return {};
        }
    }
};
} // namespace usdBVHPlugin