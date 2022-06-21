/*
Class: Vector


Timestamp: 05/12/2022 11:33:56 PM
*/

#pragma once

#ifndef DataStructuresString_HPP
#define DataStructuresString_HPP

#include <EASTL/string.h>

#ifdef LINA_ENABLE_LOGGING
#include <fmt/format.h>
#endif

#include <functional>
namespace Lina
{
    typedef eastl::string String;

    #define TO_STRING(...) eastl::to_string(__VA_ARGS__)

} // namespace Lina

template<> struct std::hash<eastl::string> {
    std::size_t operator()(eastl::string const& s) const noexcept {
        return eastl::hash<eastl::string>()(s);
    }
};

#ifdef LINA_ENABLE_LOGGING

template <>
struct fmt::formatter<eastl::string>
{
    // Presentation format: 'f' - fixed, 'e' - exponential.
    char presentation = 'f';

    // Parses format specifications of the form ['f' | 'e'].
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        // [ctx.begin(), ctx.end()) is a character range that contains a part of
        // the format string starting from the format specifications to be parsed,
        // e.g. in
        //
        //   fmt::format("{:f} - point of interest", point{1, 2});
        //
        // the range will contain "f} - point of interest". The formatter should
        // parse specifiers until '}' or the end of the range. In this example
        // the formatter should parse the 'f' specifier and return an iterator
        // pointing to '}'.

        // Please also note that this character range may be empty, in case of
        // the "{}" format string, so therefore you should check ctx.begin()
        // for equality with ctx.end().

        // Parse the presentation format and store it in the formatter:
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'f' || *it == 'e'))
            presentation = *it++;

        // Check if reached the end of the range:
        if (it != end && *it != '}')
            throw format_error("invalid format");

        // Return an iterator past the end of the parsed range:
        return it;
    }

    // Formats the point p using the parsed format specification (presentation)
    // stored in this formatter.
    template <typename FormatContext>
    auto format(const eastl::string& str, FormatContext& ctx) const -> decltype(ctx.out())
    {
        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), str.c_str());
    }
};

#endif

#endif
