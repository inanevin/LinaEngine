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

#ifndef LinaEditor_HPP
#define LinaEditor_HPP

#include "EditorRenderer.hpp"
#include "EditorGUIManager.hpp"
#include "ShortcutManager.hpp"
#include "GUI/GUI.hpp"

namespace Lina
{
    class Engine;

    namespace Event
    {
        struct ELevelInstalled;
        struct EPreMainLoop;
    } // namespace Event

    namespace World
    {
        class Entity;
        class EntityWorld;
        class LevelManager;
    } // namespace World

    namespace Resources
    {
        class EditorResourceLoader;
    }
} // namespace Lina

namespace Lina::Editor
{
    class Editor
    {
    public:
        Editor()          = default;
        virtual ~Editor() = default;

        void                OnLevelInstalled(const Event::ELevelInstalled& ev);
        void                Initialize(World::LevelManager* lvlManager, Engine* engine, Graphics::Swapchain* swp, Graphics::GUIBackend* guiBackend);
        void                Shutdown();
        void                VerifyStaticResources();
        void                CreateEditorCamera();
        void                DeleteEditorCamera();
        void                SaveCurrentLevel();
        void                PackageProject();
        void                SetPlayMode(bool enabled);
        void                SetIsPaused(bool paused);
        void                SkipNextFrame();
        Vector<const char*> GetDefaultTextures();
        Vector<const char*> GetDefaultFonts();

    private:
        void OnPreMainLoop(const Event::EPreMainLoop& ev);

    private:
        Graphics::GUIBackend*            m_guiBackend = nullptr;
        Resources::EditorResourceLoader* m_resLoader;
        World::Entity*                   m_editorCamera = nullptr;
        EditorRenderer                   m_renderer;
        EditorGUIManager                 m_guiManager;
        ShortcutManager                  m_shortcutManager;
        ImmediateGUI                     m_gui;
        World::LevelManager*             m_levelManager = nullptr;
        World::EntityWorld*              m_world        = nullptr;
        Engine*                          m_engine       = nullptr;
    };
} // namespace Lina::Editor

#endif
