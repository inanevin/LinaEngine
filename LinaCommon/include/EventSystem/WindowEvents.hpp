/*
Class: WindowEvents



Timestamp: 12/25/2021 12:20:00 PM
*/

#pragma once

#ifndef WindowEvents_HPP
#define WindowEvents_HPP

// Headers here.
#include "Core/CommonWindow.hpp"

namespace Lina::Event
{
    struct EWindowContextCreated
    {
        void* m_window;
    };
    struct EWindowResized
    {
        void*            m_window;
        WindowProperties m_windowProps;
    };
    struct EWindowClosed
    {
        void* m_window;
    };
    struct EWindowFocused
    {
        void* m_window;
        int   m_focused;
    };
    // struct EWindowMaximized { void* m_window; int m_isMaximized; };
    // struct EWindowIconified { void* m_window; int m_isIconified; };
    // struct EWindowRefreshed { void* m_window; };
    // struct EWindowMoved { void* m_window; int m_x; int m_y; };

} // namespace Lina::Event

#endif
