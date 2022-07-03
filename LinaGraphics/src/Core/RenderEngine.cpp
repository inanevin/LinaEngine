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

#include "Core/RenderEngine.hpp"
#include "Log/Log.hpp"
#include "ECS/Components/DecalComponent.hpp"
#include "ECS/Components/SkyComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/MeshComponent.hpp"
#include "ECS/Components/SpriteComponent.hpp"
#include "ECS/Components/ParticleComponent.hpp"

namespace Lina::Graphics
{
    void RenderEngine::Initialize(ApplicationInfo& appInfo)
    {
        LINA_TRACE("[Initialization] -> Render Engine ({0})", typeid(*this).name());
        m_appInfo = appInfo;

        m_skySystem.Initialize("Sky System");
        m_particleSystem.Initialize("Particle System");
        m_decalSystem.Initialize("Decal System");
        m_spriteSystem.Initialize("Sprite System");
        m_lightingSystem.Initialize("Lighting System");
        m_meshSystem.Initialize("Mesh System");
    }

    void RenderEngine::SyncRenderData()
    {
        PROFILER_FUNC(PROFILER_THREAD_SIMULATION);

        m_systemList.UpdateSystems(0.0f);
    }

    void RenderEngine::Clear()
    {
    }

    void RenderEngine::Render()
    {
        PROFILER_SCOPE_START("Render Sky", PROFILER_THREAD_RENDER);
        m_skySystem.Render();
        PROFILER_SCOPE_END("Render Sky", PROFILER_THREAD_RENDER);

        PROFILER_SCOPE_START("Render Particles", PROFILER_THREAD_RENDER);
        m_particleSystem.Render();
        PROFILER_SCOPE_END("Render Particles", PROFILER_THREAD_RENDER);

        PROFILER_SCOPE_START("Render Decals", PROFILER_THREAD_RENDER);
        m_decalSystem.Render();
        PROFILER_SCOPE_END("Render Decals", PROFILER_THREAD_RENDER);

        PROFILER_SCOPE_START("Render Sprites", PROFILER_THREAD_RENDER);
        m_spriteSystem.Render();
        PROFILER_SCOPE_END("Render Sprites", PROFILER_THREAD_RENDER);

        PROFILER_SCOPE_START("Render Meshes", PROFILER_THREAD_RENDER);
        m_meshSystem.Render();
        PROFILER_SCOPE_END("Render Meshes", PROFILER_THREAD_RENDER);
    }

    void RenderEngine::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> Render Engine ({0})", typeid(*this).name());
    }

} // namespace Lina::Graphics
