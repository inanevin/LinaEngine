/*
Class: Engine



Timestamp: 12/14/2021 10:09:33 PM
*/

#pragma once

#ifndef Engine_HPP
#define Engine_HPP

// Headers here.
#include "Core/AudioEngine.hpp"
#include "Core/InputEngine.hpp"
#include "Core/MessageBus.hpp"
#include "Core/PhysicsEngine.hpp"
#include "World/LevelManager.hpp"
#include "Core/ResourceStorage.hpp"
#include "ECS/System.hpp"
#include "ECS/SystemList.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Data/Vector.hpp"

#define DELTA_TIME_HISTORY 11

namespace Lina
{
    class Application;
    class EngineSettings;

    class Engine
    {

    public:
        static Engine* Get()
        {
            return s_engine;
        }

        void   PackageProject(const String& path);
        double GetElapsedTime();
        void   SetPlayMode(bool enabled);
        void   SetIsPaused(bool paused);
        void   SkipNextFrame();

        inline EngineSettings* GetEngineSettings()
        {
            return m_engineSettings;
        }
        inline void AddToMainPipeline(ECS::System& system)
        {
            m_mainECSPipeline.AddSystem(system);
        }
        inline int GetCurrentFPS()
        {
            return m_currentFPS;
        }
        inline int GetCurrentUPS()
        {
            return m_currentUPS;
        }
        inline double GetRawDelta()
        {
            return m_rawDeltaTime;
        }
        inline double GetSmoothDelta()
        {
            return m_smoothDeltaTime;
        }
        inline double GetFrameTime()
        {
            return m_frameTime;
        }
        inline double GetRenderTime()
        {
            return m_renderTime;
        }
        inline double GetUpdateTime()
        {
            return m_updateTime;
        }
        inline bool GetPlayMode()
        {
            return m_isInPlayMode;
        }
        inline bool GetIsPaused()
        {
            return m_paused;
        }
        inline bool GetPauseMode()
        {
            return m_paused;
        }
        inline const ECS::SystemList& GetPipeline()
        {
            return m_mainECSPipeline;
        }
        inline ApplicationInfo GetAppInfo()
        {
            return m_appInfo;
        }

    private:
        friend class Application;

        Engine()  = default;
        ~Engine() = default;

        void   Initialize(ApplicationInfo& appInfo);
        void   Run();
        void   UpdateGame(float deltaTime);
        void   DisplayGame(float interpolation);
        void   RemoveOutliers(bool biggest);
        void   RegisterResourceTypes();
        double SmoothDeltaTime(double dt);

    private:
        static Engine*             s_engine;
        Resources::ResourceStorage m_resourceStorage;
        Physics::PhysicsEngine     m_physicsEngine;
        Audio::AudioEngine         m_audioEngine;
        Input::InputEngine         m_inputEngine;
        Event::EventSystem         m_eventSystem;
        ECS::SystemList            m_mainECSPipeline;
        World::LevelManager        m_levelManager;
        MessageBus                 m_messageBus;
        ApplicationInfo            m_appInfo;
        bool                       m_running         = false;
        bool                       m_canRender       = true;
        bool                       m_firstRun        = true;
        bool                       m_isInPlayMode    = false;
        bool                       m_paused          = false;
        bool                       m_shouldSkipFrame = false;

        // Performance & variable stepping
        int                                    m_currentFPS = 0;
        int                                    m_currentUPS = 0;
        double                                 m_updateTime = 0;
        double                                 m_renderTime = 0;
        double                                 m_frameTime  = 0;
        double                                 m_smoothDeltaTime;
        double                                 m_rawDeltaTime;
        std::array<double, DELTA_TIME_HISTORY> m_deltaTimeArray;
        uint8                                  m_deltaTimeArrIndex  = 0;
        uint8                                  m_deltaTimeArrOffset = 0;
        int                                    m_deltaFirstFill     = 0;
        bool                                   m_deltaFilled        = false;
        double                                 m_startTime          = 0.0;
        float                                  m_physicsAccumulator = 0.0f;
        EngineSettings*                        m_engineSettings     = nullptr;
    };
} // namespace Lina

#endif
