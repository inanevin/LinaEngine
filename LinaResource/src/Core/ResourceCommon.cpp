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

#include "Core/ResourceCommon.hpp"

namespace Lina::Resources
{
    const char* GetPackageTypeName(PackageType type)
    {
        switch (type)
        {
        case PackageType::Static:
            return "static";
        case PackageType::Audio:
            return "audio";
        case PackageType::Graphics:
            return "graphics";
        case PackageType::Level:
            return "levels";
        case PackageType::Models:
            return "models";
        case PackageType::Textures:
            return "textures";
        case PackageType::Physics:
            return "physics";
        default:
            "none";
        }
    }

    uint32 GetPackageVersion(PackageType type)
    {
        switch (type)
        {
        case PackageType::Static:
            return 0;
        case PackageType::Audio:
            return 0;
        case PackageType::Graphics:
            return 0;
        case PackageType::Level:
            return 0;
        case PackageType::Models:
            return 0;
        case PackageType::Textures:
            return 0;
        case PackageType::Physics:
            return 0;
        default:
            return 0;
        }

        return 0;
    }
} // namespace Lina::Resources
