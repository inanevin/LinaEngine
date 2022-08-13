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

#include "Renderer.hpp"
#include "Data/Deque.hpp"
#include "Utility/StringId.hpp"
#include "Data/FixedVector.hpp"
#include "ECS/SystemList.hpp"
#include "Core/CommonApplication.hpp"
#include "Utility/DeletionQueue.hpp"
#include "Backend.hpp"
#include "Window.hpp"
#include "FeatureRenderers/StaticMeshRenderer.hpp"
#include "Views/PlayerView.hpp"
#include <functional>

namespace Lina
{
    namespace Event
    {
        struct ESwapchainRecreated;
        struct EPreStartGame;
    } // namespace Event
} // namespace Lina

namespace Lina::Graphics
{
    class FeatureRenderer;

    class RenderEngine
    {

    public:
        RenderEngine()  = default;
        ~RenderEngine() = default;

        static inline RenderEngine* Get()
        {
            return s_instance;
        }

        void Initialize(ApplicationInfo& appInfo);
        void Clear();
        void Render();
        void Join();
        void Shutdown();
        void FetchVisibilityState();
        void ExtractGameState();
        void PrepareRenderData();
        void OnPreStartGame(const Event::EPreStartGame& ev);
        void OnSwapchainRecreated(const Event::ESwapchainRecreated& ev);

        inline Renderer& GetRenderer()
        {
            return m_renderer;
        }

        inline DeletionQueue& GetMainDeletionQueue()
        {
            return m_mainDeletionQueue;
        }

        inline bool IsInitialized()
        {
            return m_initedSuccessfully;
        }

    private:
        friend class Engine;

        Vector<FeatureRenderer*> m_featureRenderers;
        static RenderEngine*     s_instance;
        DeletionQueue            m_mainDeletionQueue;
        ApplicationInfo          m_appInfo;
        Window                   m_window;
        Backend                  m_backend;
        bool                     m_initedSuccessfully = false;

        PlayerView         m_playerView;
        Renderer           m_renderer;
        StaticMeshRenderer m_meshRenderer;
    };
} // namespace Lina::Graphics

#endif
