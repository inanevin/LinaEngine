/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

/*
Class: GUILayer

This class is pushed as an overlay layer to the render engine and is responsible for drawing the editor.
It inits panels, drawers etc. and is the main bridge of communication between editor components.

*/

#ifndef GUILAYER_HPP
#define GUILAYER_HPP

#include "Panels/ECSPanel.hpp"
#include "Panels/GlobalSettingsPanel.hpp"
#include "Panels/HeaderPanel.hpp"
#include "Panels/LevelPanel.hpp"
#include "Panels/LogPanel.hpp"
#include "Panels/MainToolbarPanel.hpp"
#include "Panels/ProfilerPanel.hpp"
#include "Panels/PropertiesPanel.hpp"
#include "Panels/ResourcesPanel.hpp"
#include "World/DefaultLevel.hpp"

#include <vector>

struct ImFont;

namespace Lina
{
    namespace World
    {
        class Level;
    }

    namespace Event
    {
        struct EShutdown;
        struct EPostRender;
        struct EResourceLoadUpdated;
    } // namespace Event
} // namespace Lina

namespace Lina::Editor
{

    class GUILayer
    {

    public:
        GUILayer()
        {
        }
        ~GUILayer(){};

        void           Initialize();
        void           OnShutdown(const Event::EShutdown& ev);
        void           OnPostRender(const Event::EPostRender&);
        static ImFont* GetDefaultFont()
        {
            return s_defaultFont;
        }
        static ImFont* GetBigFont()
        {
            return s_bigFont;
        }
        static std::map<const char*, EditorPanel*> s_editorPanels;

        // Menu bar item callback from header panel.
        void DispatchMenuBarClickedAction(const EMenuBarItemClicked& event);

        void        Refresh();
        LevelPanel& GetLevelPanel()
        {
            return m_levelPanel;
        }

    private:
        void DrawSplashScreen();
        void OnResourceLoadUpdated(const Event::EResourceLoadUpdated& ev);

        void DrawFPSCounter(int corner = 0);
        void DrawCentralDockingSpace();
        void CreateObjectInLevel(const std::string& modelPath);

    private:
        static ImFont* s_defaultFont;
        static ImFont* s_bigFont;

        MainToolbarPanel    m_toolbar;
        ECSPanel            m_ecsPanel;
        ResourcesPanel      m_resourcesPanel;
        LevelPanel          m_levelPanel;
        PropertiesPanel     m_propertiesPanel;
        LogPanel            m_logPanel;
        HeaderPanel         m_headerPanel;
        ProfilerPanel       m_profilerPanel;
        GlobalSettingsPanel m_globalSettingsPanel;
        World::DefaultLevel m_defaultLevel;
        std::string         m_currentlyLoadingResource = "";
        float               m_percentage               = 0.0f;
    };
} // namespace Lina::Editor

#endif
