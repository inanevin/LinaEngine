/*
Class: ECSComponent

Defines the base component class as well as macros for drawing components
via the sandbox editor.

Timestamp: 5/23/2020 5:17:02 PM

*/

#pragma once

#ifndef ECSComponent_HPP
#define ECSComponent_HPP

#include "Core/CommonReflection.hpp"

namespace Lina
{
    class ReflectionRegistry;

} // namespace Lina

namespace Lina::ECS
{
    class Component
    {

    public:
        Component() = default;
        virtual ~Component(){};

        inline virtual void SetIsEnabled(bool enabled)
        {
            m_isEnabled = enabled;
        }

        inline bool GetIsEnabled()
        {
            return m_isEnabled;
        }

    protected:

        friend class ReflectionRegistry;
        bool m_isEnabled = true;
    };
} // namespace Lina::ECS

#endif
