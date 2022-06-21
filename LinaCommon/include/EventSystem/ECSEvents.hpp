/*
Class: EntityEvents



Timestamp: 12/28/2021 2:58:34 PM
*/

#pragma once

#ifndef EntityEvents_HPP
#define EntityEvents_HPP

#include "Core/CommonECS.hpp"

namespace Lina::Event
{
    struct EEntityEnabledChanged
    {
        ECS::Entity m_entity;
        bool        m_enabled;
    };

} // namespace Lina::Event

#endif
