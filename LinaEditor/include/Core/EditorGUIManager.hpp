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

#ifndef EditorGUIManager_HPP
#define EditorGUIManager_HPP

#include "Utility/Graphics/TexturePacker.hpp"
#include "EditorCommon.hpp"
#include "Utility/StringId.hpp"
#include "GUI/GUI.hpp"
#include "Core/CommonEngine.hpp"

namespace Lina
{
    namespace Event
    {
        struct EEngineResourcesLoaded;
        struct ETick;
        struct ESyncData;
    } // namespace Event

    namespace Graphics
    {
        class GUIBackend;
        class Swapchain;
        class Font;
        class WindowManager;
    } // namespace Graphics

} // namespace Lina

namespace Lina::Editor
{
    struct EShortcut;
    class Drawable;
    class DockArea;
    class TopPanel;

    class EditorGUIManager
    {

    private:
    public:
        void Initialize(const EngineSubsystems& subsys);
        void Shutdown();
        void LaunchPanel(EditorPanel panel);
        void DestroyDockArea(StringID sid);

        inline Graphics::Texture* GetIconTexture()
        {
            return m_iconTexture;
        }

    private:
        void      OnTick(const Event::ETick& ev);
        void      OnSyncData(const Event::ESyncData& ev);
        void      FindHoveredSwapchain();
        void      CreateImmediateGUI();
        Drawable* GetContentFromPanelRequest(EditorPanel panel);

    private:
        Graphics::Texture*    m_iconTexture = nullptr;
        Vector<PackedTexture> m_packedIcons;
        Vector<DockArea*>     m_additionalDockAreas;
        DockArea*             m_mainDockArea       = nullptr;
        TopPanel*             m_topPanel           = nullptr;
        StringID              m_hoveredSwapchainID = 0;
        StringID              m_topMostSwapchainID = 0;
        Vector<ImmediateGUI>  m_guis;
        Graphics::Font*       m_rubikFont  = nullptr;
        Graphics::Font*       m_nunitoFont = nullptr;
        EngineSubsystems      m_subsys;
    };
} // namespace Lina::Editor

#endif
