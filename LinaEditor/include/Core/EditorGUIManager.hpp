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

namespace Lina
{
    namespace Event
    {
        struct EEngineResourcesLoaded;
        struct EDrawGUI;
        struct ETick;
    } // namespace Event

    namespace Graphics
    {
        class GUIBackend;
        class Swapchain;
        class WindowManager;
    } // namespace Graphics

} // namespace Lina

namespace Lina::Editor
{
    struct EShortcut;
    class Drawable;
    class DockArea;
    class TopPanel;
    class EditorRenderer;

    class EditorGUIManager
    {

    private:
        struct LaunchPanelRequest
        {
            EditorPanel panelType = EditorPanel::Global;
            StringID    sid       = 0;
            Vector2     pos       = Vector2::Zero;
            Vector2     size      = Vector2::Zero;
        };

    public:
        void Initialize(Graphics::GUIBackend* guiBackend, EditorRenderer* rend, Graphics::WindowManager* wm);
        void Shutdown();
        void LaunchPanel(EditorPanel panel);

        inline void SetCurrentSwapchain(Graphics::Swapchain* swp)
        {
            m_currentSwapchain = swp;
        }

        inline Graphics::Texture* GetIconTexture()
        {
            return m_iconTexture;
        }

    private:
        void      OnTick(const Event::ETick& ev);
        void      OnDrawGUI(const Event::EDrawGUI& ev);
        void      FindHoveredSwapchain();
        Drawable* GetContentFromPanelRequest(EditorPanel panel);

    private:
        Graphics::GUIBackend*      m_guiBackend    = nullptr;
        Graphics::Texture*         m_iconTexture   = nullptr;
        Graphics::WindowManager*   m_windowManager = nullptr;
        Vector<PackedTexture>      m_packedIcons;
        EditorRenderer*            m_renderer = nullptr;
        Vector<DockArea*>          m_dockAreas;
        Graphics::Swapchain*       m_currentSwapchain = nullptr;
        DockArea*                  m_mainDockArea     = nullptr;
        TopPanel*                  m_topPanel         = nullptr;
        Vector<LaunchPanelRequest> m_panelRequests;
        StringID                   m_hoveredSwapchainID = 0;
    };
} // namespace Lina::Editor

#endif
