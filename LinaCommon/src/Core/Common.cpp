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

#include "Core/CommonApplication.hpp"
#include "Core/CommonPhysics.hpp"
#include "Core/CommonUtility.hpp"


namespace EA
{
    namespace StdC
    {
        // Provided by the EAStdC package or by the user.
        int Vsnprintf(char* EA_RESTRICT pDestination, size_t n, const char* EA_RESTRICT pFormat, va_list arguments)
        {
        return vsnprintf(pDestination, n, pFormat, arguments);
        }
        int Vsnprintf(char16_t* EA_RESTRICT pDestination, size_t n, const char16_t* EA_RESTRICT pFormat, va_list arguments)
        {
            return 0;
        }
        int Vsnprintf(char32_t* EA_RESTRICT pDestination, size_t n, const char32_t* EA_RESTRICT pFormat, va_list arguments)
        {
            return 0;
        }
#if EA_CHAR8_UNIQUE
        int Vsnprintf(char8_t* EA_RESTRICT pDestination, size_t n, const char8_t* EA_RESTRICT pFormat, va_list arguments)
        {
            return 0;
        }
#endif
#if defined(EA_WCHAR_UNIQUE) && EA_WCHAR_UNIQUE
        int Vsnprintf(wchar_t* EA_RESTRICT pDestination, size_t n, const wchar_t* EA_RESTRICT pFormat, va_list arguments)
        {
            return 0;
        }
#endif
    } // namespace StdC
} // namespace EA

// EASTL OPERATOR NEW[] REQUIREMENTS

namespace Lina
{
    String LogLevelAsString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::Critical:
            return "Critical";
        case LogLevel::Debug:
            return "Debug";
        case LogLevel::Error:
            return "Error";
        case LogLevel::Info:
            return "Info";
        case LogLevel::None:
            return "None";
        case LogLevel::Trace:
            return "Trace";
        case LogLevel::Warn:
            return "Warn";
        default:
            return "";
        }
    }
} // namespace Lina

namespace Lina::Physics
{
    String SIMULATION_TYPES[3] = {"None", "Static", "Dynamic"};
    String COLLISION_SHAPES[4] = {"Box", "Sphere", "Capsule", "ConvexMesh"};
} // namespace Lina::Physics

namespace Lina::Utility
{
    Folder::~Folder()
    {
        for (int i = 0; i < files.size(); i++)
        {
            delete files[i];
        }

        files.clear();

        for (int i = 0; i < folders.size(); i++)
            delete folders[i];

        folders.clear();
    }
} // namespace Lina::Utility