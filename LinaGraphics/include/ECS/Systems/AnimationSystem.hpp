/*
Class: AnimationSystem



Timestamp: 12/7/2021 4:10:26 PM
*/

#pragma once

#ifndef AnimationSystem_HPP
#define AnimationSystem_HPP

// Headers here.
#include "ECS/System.hpp"

namespace Lina
{
    namespace Graphics
    {
        class RenderEngine;
    }
} // namespace Lina
namespace Lina::ECS
{
    class AnimationSystem : public System
    {

    public:
        AnimationSystem()  = default;
        ~AnimationSystem() = default;

        virtual void Initialize(const String& name) override;
        virtual void UpdateComponents(float delta) override;

    protected:
    };
} // namespace Lina::ECS

#endif
