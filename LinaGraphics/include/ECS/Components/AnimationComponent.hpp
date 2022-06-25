/*
Class: AnimationComponent



Timestamp: 12/7/2021 4:13:25 PM
*/

#pragma once

#ifndef AnimationComponent_HPP
#define AnimationComponent_HPP

// Headers here.
#include "ECS/Component.hpp"

#include <Data/String.hpp>

namespace Lina::ECS
{
    struct AnimationComponent : public Component
    {
        String m_animationName;
    };
} // namespace Lina::ECS

#endif
