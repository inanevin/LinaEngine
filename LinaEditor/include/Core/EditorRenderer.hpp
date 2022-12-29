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

#ifndef EditorRenderer_HPP
#define EditorRenderer_HPP

#include "Graphics/Core/GameRenderer.hpp"

namespace Lina
{
    namespace World
    {
        class EntityWorld;
    }

    namespace Event
    {
        struct EWorldDestroyed;
    }
    namespace Graphics
    {
        class GUIBackend;
        class CameraComponent;
    } // namespace Graphics
} // namespace Lina

namespace Lina::Editor
{
    class EditorRenderer : public Graphics::GameRenderer
    {

    public:
        inline const HashMap<World::EntityWorld*, Graphics::GameRenderer::RenderWorldData>& GetWorldData()
        {
            return m_worldDataCPU;
        }

    protected:
        friend class Editor;

        virtual void CreateRenderPasses() override;
        virtual void Tick() override;
        virtual void Render() override;
        virtual void SyncData() override;
        virtual void OnComponentCreated(const Event::EComponentCreated& ev) override;
        virtual void OnComponentDestroyed(const Event::EComponentDestroyed& ev) override;
        virtual void OnLevelUninstalled(const Event::ELevelUninstalled& ev) override;
        virtual void OnPreMainLoop(const Event::EPreMainLoop& ev) override;
        virtual void ConnectEvents() override;

        void OnWorldDestroyed(const Event::EWorldDestroyed& ev);

    private:
        HashMap<World::EntityWorld*, Graphics::GameRenderer::RenderWorldData> m_worldDataCPU;
        HashMap<World::EntityWorld*, Graphics::GameRenderer::RenderWorldData> m_worldDataGPU;
    };
} // namespace Lina::Editor

#endif
