/*
Class: MainLoopEvents



Timestamp: 12/25/2021 12:17:16 PM
*/

#pragma once

#ifndef MainLoopEvents_HPP
#define MainLoopEvents_HPP

// Headers here.
#include "Core/CommonApplication.hpp"

namespace Lina::Event
{
    struct EShutdown
    {
    };
    struct EStartGame
    {
    };
    struct EEndGame
    {
    };
    struct ETick
    {
        float m_deltaTime;
        bool  m_isInPlayMode;
    };
    struct EPreTick
    {
        float m_deltaTime;
        bool  m_isInPlayMode;
    };
    struct EPostTick
    {
        float m_deltaTime;
        bool  m_isInPlayMode;
    };
    struct ERender
    {
    };
    struct EPlayModeChanged
    {
        bool m_playMode;
    };
    struct EPauseModeChanged
    {
        bool m_isPaused;
    };
} // namespace Lina::Event

#endif
