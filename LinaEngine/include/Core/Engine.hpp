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

#ifndef Engine_HPP
#define Engine_HPP

// Headers here.
#include "Audio/AudioEngine.hpp"
#include "Input/Core/InputEngine.hpp"
#include "Physics/Core/PhysicsEngine.hpp"
#include "Graphics/Core/RenderEngine.hpp"
#include "World/Core/LevelManager.hpp"
#include "Resource/Core/ResourceManager.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Data/Vector.hpp"
#include "JobSystem/JobSystem.hpp"
#include "Memory/MemoryManager.hpp"

#ifndef LINA_PRODUCTION
#include "Core/Editor.hpp"
#endif

#define DELTA_TIME_HISTORY 11

namespace Graphics
{
    class Renderer;
}
namespace Lina
{
    class Application;
    class EngineSettings;
    class RenderSettings;
    class GameManager;

    class Engine
    {
    public:
        /// <summary>
        /// Tries to cap the application to a certain frames-per-second. Accurate up to 240~ frames depending on the platform.
        /// Setting 0 removes the cap.
        /// Frame limit is first and foremost depending on GPU and vsync mode. After rendering and refreshing, if there are still some time to cap,
        /// then this frame limit is used.
        /// </summary>
        /// <param name="maxFPS"></param>
        inline void SetFrameLimit(int maxFPS)
        {
            m_frameLimit        = maxFPS;
            m_frameLimitSeconds = 1.0 / static_cast<double>(m_frameLimit);
        }

        inline const EngineSettings& GetEngineSettings()
        {
            return *m_engineSettings;
        }

        inline const RenderSettings& GetRenderSettings()
        {
            return *m_renderSettings;
        }

    private:
        friend class Application;

        Engine()  = default;
        ~Engine() = default;

        void   Initialize(const InitInfo& initInfo, GameManager* gm);
        void   LoadEngineResources();
        void   Start();
        void   Tick();
        void   Shutdown();
        void   RunSimulation(float deltaTime);
        void   RemoveOutliers(bool biggest);
        void   RegisterResourceTypes();
        double SmoothDeltaTime(double dt);

    private:
        Resources::ResourceManager m_resourceManager;
        Physics::PhysicsEngine     m_physicsEngine;
        Audio::AudioEngine         m_audioEngine;
        Input::InputEngine         m_inputEngine;
        Event::EventSystem         m_eventSystem;
        World::LevelManager        m_levelManager;
        Graphics::RenderEngine     m_renderEngine;
        Memory::MemoryManager      m_memoryManager;
        JobSystem                  m_jobSystem;
        GameManager*               m_gameManager       = nullptr;
        bool                       m_canRender         = true;
        bool                       m_firstRun          = true;
        int                        m_frameLimit        = 0;
        double                     m_frameLimitSeconds = 0;
        String                     m_initialTitle      = "";

        // Performance & variable stepping
        bool                                   m_shouldSkipFrame    = false;
        bool                                   m_paused             = false;
        bool                                   m_isInPlayMode       = false;
        int                                    m_currentFPS         = 0;
        int                                    m_currentUPS         = 0;
        double                                 m_updateTime         = 0;
        double                                 m_renderTime         = 0;
        double                                 m_frameTime          = 0;
        int                                    m_frames             = 0;
        int                                    m_updates            = 0;
        double                                 m_totalFPSTime       = 0.0;
        double                                 m_previousFrameTime  = 0.0;
        double                                 m_currentFrameTime   = 0.0;
        uint8                                  m_deltaTimeArrIndex  = 0;
        uint8                                  m_deltaTimeArrOffset = 0;
        int                                    m_deltaFirstFill     = 0;
        bool                                   m_deltaFilled        = false;
        float                                  m_physicsAccumulator = 0.0f;
        EngineSettings*                        m_engineSettings     = nullptr;
        RenderSettings*                        m_renderSettings     = nullptr;
        Graphics::Renderer*                    m_defaultRenderer    = nullptr;
        Future<void>                           m_renderJob;
        std::array<double, DELTA_TIME_HISTORY> m_deltaTimeArray;

#ifndef LINA_PRODUCTION
        Editor::Editor m_editor;
#endif
    };
} // namespace Lina

#endif
