/*
Class: EditorPanel

Base class defining functions for all panels.

Timestamp: 5/23/2020 4:16:05 PM
*/

#pragma once

#ifndef EditorPanel_HPP
#define EditorPanel_HPP

#include "Math/Vector.hpp"

struct ImGuiWindow;

namespace Lina::Editor
{
    class GUILayer;

    class EditorPanel
    {

    public:
        EditorPanel()          = default;
        virtual ~EditorPanel() = default;
        virtual void Initialize(const char* id, const char* icon);
        virtual void Draw() = 0;
        virtual bool Begin();
        virtual void End();
        virtual void Open()
        {
            m_show = true;
        }
        virtual void Close()
        {
            m_show = false;
        }
        virtual void ToggleCollapse();
        virtual void ToggleMaximize();
        virtual bool CanDrawContent();
        virtual bool IsMaximized()
        {
            return m_maximized;
        }
        virtual bool IsCollapsed()
        {
            return m_collapsed;
        }
        inline Vector2 GetCurrentWindowPos()
        {
            return m_currentWindowPos;
        }
        inline Vector2 GetcurrentWindowSize()
        {
            return m_currentWindowSize;
        }
        inline bool IsDocked()
        {
            return m_isDocked;
        }
        inline String GetTitle()
        {
            return m_title;
        }

    protected:
        float       m_dpiScale               = 0.0f;
        bool        m_isDocked               = false;
        bool        m_show                   = true;
        bool        m_collapsed              = false;
        bool        m_maximized              = false;
        Vector2     m_sizeBeforeMaximize     = Vector2::Zero;
        Vector2     m_sizeBeforeCollapse     = Vector2::Zero;
        Vector2     m_posBeforeMaximize      = Vector2::Zero;
        bool        m_lockWindowPos          = false;
        const char* m_id                     = nullptr;
        const char* m_icon                   = nullptr;
        String m_title                  = "";
        int         m_windowFlags            = 0;
        bool        m_setWindowSizeNextFrame = false;
        bool        m_setWindowPosNextFrame  = false;
        Vector2     m_windowPosNextFrame     = Vector2::Zero;
        Vector2     m_windowSizeNextFrame    = Vector2::Zero;
        Vector2     m_currentWindowPos       = Vector2::Zero;
        Vector2     m_currentWindowSize      = Vector2::Zero;
    };
} // namespace Lina::Editor

#endif
