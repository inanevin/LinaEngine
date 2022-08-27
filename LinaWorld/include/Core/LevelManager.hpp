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

#ifndef LevelManager_HPP
#define LevelManager_HPP

#include "Data/HashMap.hpp"
#include "Utility/StringId.hpp"

namespace Lina
{
    namespace Editor
    {
        class EditorManager;
    }
} // namespace Lina

namespace Lina::World
{
    class Level;

    class LevelManager
    {
    public:
        static LevelManager* Get()
        {
            return s_instance;
        }

        inline Level* GetCurrentLevel()
        {
            return m_currentLevel;
        }

    private:
        LevelManager()          = default;
        virtual ~LevelManager() = default;

        void Initialize();
        void Shutdown();
        void UninstallCurrent();
        void SaveCurrentLevel();

        /// <summary>
        /// Async loading will install the level & call ELevelInstalled immediately.
        /// Check for level's readyness and ELevelResourcesLoaded event to know when the level is playable.
        /// Normal loading will wait for the resources to load first.
        /// </summary>
        /// <param name="path"></param>
        /// <param name="loadAsync"></param>
        void InstallLevel(const String& path, bool loadAsync);

        /// <summary>
        /// !Works in editor only!
        /// </summary>
        /// <param name="path"></param>
        void CreateLevel(const String& path);

        friend class Engine;
        friend class EntityWorld;
        friend class Editor::EditorManager;

        static LevelManager* s_instance;
        Level*               m_currentLevel = nullptr;
    };
} // namespace Lina::World

#endif
