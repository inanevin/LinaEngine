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

/*
Class: EngineSettings



Timestamp: 1/6/2022 6:58:17 PM
*/

#pragma once

#ifndef EngineSettings_HPP
#define EngineSettings_HPP

// Headers here.
#include "Resources/ResourceHandle.hpp"
#include "World/Level.hpp"

namespace Lina
{
    namespace Editor
    {
        class ClassDrawer;
    }
}

namespace Lina
{
    class Engine;

    LINA_CLASS("Engine Settings")
    class EngineSettings
    {

    public:
        EngineSettings()  = default;
        ~EngineSettings() = default;


        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_startupLevel);
        }

    private:
        friend class Engine;
        friend class Editor::ClassDrawer;
        Resources::ResourceHandle<World::Level> m_startupLevel;
    };
} // namespace Lina

#endif
