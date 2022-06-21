/*
Class: InputEvents



Timestamp: 12/25/2021 12:20:28 PM
*/

#pragma once

#ifndef InputEvents_HPP
#define InputEvents_HPP

// Headers here.
#include "Core/CommonInput.hpp"

namespace Lina::Event
{
    struct EKeyCallback
    {
        void*              m_window;
        int                m_key;
        int                m_scancode;
        Input::InputAction m_action;
        int                m_mods;
    };
    struct EMouseButtonCallback
    {
        void*              m_window;
        int                m_button;
        Input::InputAction m_action;
        int                m_mods;
    };
    struct EMouseScrollCallback
    {
        void*  m_window;
        double m_xoff;
        double m_yoff;
    };
    struct EMouseCursorCallback
    {
        void*  m_window;
        double m_xpos;
        double m_ypos;
    };
} // namespace Lina::Event

#endif
