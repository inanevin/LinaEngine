/*
Class: LevelManager

Timestamp: 5/6/2022 5:10:23 PM
*/

#pragma once

#ifndef LevelManager_HPP
#define LevelManager_HPP

#include "Core/CommonApplication.hpp"
#include "Data/HashMap.hpp"
#include "Utility/StringId.hpp"

namespace Lina::World
{
    class Level;

    class LevelManager
    {
    public:
        LevelManager()          = default;
        virtual ~LevelManager() = default;

        static LevelManager* Get()
        {
            return s_instance;
        }

        void Initialize(ApplicationInfo appInfo);
        void Shutdown();
        void InstallLevel(const String& path);
        void SaveCurrentLevel();

        /// <summary>
        /// !Works in editor only!
        /// </summary>
        /// <param name="path"></param>
        void CreateLevel(const String& path);

        inline Level* GetCurrentLevel()
        {
            return m_currentLevel;
        }

    private:
        friend class Engine;

        static LevelManager* s_instance;
        ApplicationInfo      m_appInfo;

        Level* m_currentLevel = nullptr;
    };
} // namespace Lina::World

#endif
