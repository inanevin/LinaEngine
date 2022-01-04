/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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
#include "Core/CommonResources.hpp"
#include "Core/CommonUtility.hpp"

namespace Lina
{
    std::string LogLevelAsString(LogLevel level)
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
    std::string SIMULATION_TYPES[3] = {"None", "Static", "Dynamic"};

#ifdef LINA_PHYSICS_BULLET
    std::string COLLISION_SHAPES[4] = {"Box", "Sphere", "Cylinder", "Capsule"};
#elif LINA_PHYSICS_PHYSX
    std::string COLLISION_SHAPES[4] = {"Box", "Sphere", "Capsule", "ConvexMesh"};
#endif
} // namespace Lina::Physics

namespace Lina::Resources
{

} // namespace Lina::Resources

namespace Lina::Utility
{
    Folder::~Folder()
    {
        for (int i = 0; i < m_files.size(); i++)
        {
            delete m_files[i];
        }

        m_files.clear();

        for (int i = 0; i < m_folders.size(); i++)
            delete m_folders[i];

        m_folders.clear();
    }
} // namespace Lina::Utility