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

#ifndef RenderEngine_HPP
#define RenderEngine_HPP

#include "Utility/StringId.hpp"
#include "Data/FixedVector.hpp"
#include "ECS/SystemList.hpp"
#include "Core/CommonApplication.hpp"
#include "ECS/Systems/ParticleSystem.hpp"
#include "ECS/Systems/SpriteSystem.hpp"
#include "ECS/Systems/DecalSystem.hpp"
#include "ECS/Systems/MeshSystem.hpp"
#include "ECS/Systems/LightingSystem.hpp"
#include "ECS/Systems/SkySystem.hpp"

namespace Lina::Graphics
{
    class RenderEngine
    {

        typedef FixedVector<TypeID, 8> GraphicsComponents;

    public:
        RenderEngine()  = default;
        ~RenderEngine() = default;

        void Initialize(ApplicationInfo& appInfo);
        void SyncRenderData();
        void Clear();
        void Render();
        void Shutdown();

    private:
        friend class Engine;

        ECS::SkySystem      m_skySystem;
        ECS::DecalSystem    m_decalSystem;
        ECS::ParticleSystem m_particleSystem;
        ECS::SpriteSystem   m_spriteSystem;
        ECS::MeshSystem     m_meshSystem;
        ECS::LightingSystem m_lightingSystem;
        ECS::SystemList     m_systemList;

        ApplicationInfo    m_appInfo;
    };
} // namespace Lina::Graphics

#endif
