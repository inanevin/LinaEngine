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

#ifndef CommonEngine_HPP
#define CommonEngine_HPP

#include "Data/HashMap.hpp"
#include "Math/Vector.hpp"
#include "Utility/StringId.hpp"
#include "Data/String.hpp"

namespace Lina
{
    namespace Audio
    {
        class AudioEngine;
    }

    namespace Graphics
    {
        class RenderEngine;
    }

    namespace Physics
    {
        class PhysicsEngine;
    }

    namespace Input
    {
        class InputEngine;
    }

    namespace Editor
    {
        class Editor;
    }

    namespace World
    {
        class LevelManager;
    }

    class Engine;

    struct EngineSubsystems
    {
        Engine*                 engine        = nullptr;
        Graphics::RenderEngine* renderEngine  = nullptr;
        Audio::AudioEngine*     audioEngine   = nullptr;
        Physics::PhysicsEngine* physicsEngine = nullptr;
        Input::InputEngine*     inputEngine   = nullptr;
        World::LevelManager*    levelManager  = nullptr;
    };

    class DefaultResources
    {
    public:
        static inline const HashMap<TypeID, Vector<String>>& GetEngineResources()
        {
            return s_engineResources;
        }

        static bool IsEngineResource(TypeID tid, StringID sid);

    private:
        friend class Engine;
        friend class Editor::Editor;
        static HashMap<TypeID, Vector<String>> s_engineResources;
    };

#define LINA_EDITOR_CAMERA_NAME "lina_entityreserved_editorcam"
} // namespace Lina

#endif
