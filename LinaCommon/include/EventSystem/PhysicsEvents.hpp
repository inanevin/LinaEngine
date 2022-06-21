/*
Class: PhysicsEvents



Timestamp: 12/25/2021 12:18:08 PM
*/

#pragma once

#ifndef PhysicsEvents_HPP
#define PhysicsEvents_HPP

// Headers here.

namespace Lina::Event
{
    struct EPhysicsTick
    {
        float m_fixedDelta;
        bool  m_isInPlayMode;
    };
    struct EPrePhysicsTick
    {
        float m_fixedDelta;
        bool  m_isInPlayMode;
    };
    struct EPostPhysicsTick
    {
        float m_fixedDelta;
        bool  m_isInPlayMode;
    };
} // namespace Lina::Event

#endif
