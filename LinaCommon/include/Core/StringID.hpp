/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#ifndef StringId_HPP
#define StringId_HPP

#include "Core/SizeDefinitions.hpp"
#include "LinaCommonExports.hpp"
#include <cstddef>

// Headers here.
namespace Lina
{
    typedef uint32 StringID;
    typedef uint32 TypeID;

    // https://gist.github.com/hwei/1950649d523afd03285c
    class LINACOMMON_API FnvHash
    {
    public:
        static const unsigned int                               FNV_PRIME    = 16777619u;
        static const unsigned int                               OFFSET_BASIS = 2166136261u;
        template <unsigned int N> static constexpr unsigned int fnvHashConst(const char (&str)[N], unsigned int I = N)
        {
            return I == 1 ? (OFFSET_BASIS ^ str[0]) * FNV_PRIME : (fnvHashConst(str, I - 1) ^ str[I - 1]) * FNV_PRIME;
        }
        static uint32 fnvHash(const char* str);
        struct Wrapper
        {
            Wrapper(const char* str) : str(str)
            {
            }
            const char* str;
        };
        unsigned int hash_value;

    public:
        // calulate in run-time
        FnvHash(Wrapper wrapper) : hash_value(fnvHash(wrapper.str))
        {
        }
        // calulate in compile-time
        template <unsigned int N> constexpr FnvHash(const char (&str)[N]) : hash_value(fnvHashConst(str))
        {
        }
        // output result
        constexpr operator unsigned int() const
        {
            return this->hash_value;
        }
    };

    template <typename T> LINACOMMON_API TypeID GetTypeID()
    {
        return FnvHash(typeid(T).name());
    }

    constexpr StringID operator"" _hs(const char* str, std::size_t) noexcept
    {
        return FnvHash(str);
    }

#define TO_SID(X)  FnvHash(X.c_str())
#define TO_SIDC(X) FnvHash(X)

} // namespace Lina

#endif
