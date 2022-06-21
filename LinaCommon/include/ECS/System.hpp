/*
Class: System



Timestamp: 12/25/2021 12:38:18 PM
*/

#pragma once

#ifndef System_HPP
#define System_HPP

#include <Data/String.hpp>

namespace Lina
{
    namespace Event
    {
        struct ELevelInstalled;
    }
}

namespace Lina::ECS
{
    class Registry;
    class System
    {

    public:
        System()          = default;
        virtual ~System() = default;

        virtual void Initialize(const String& name);
        virtual void UpdateComponents(float delta) = 0;

        virtual int GetPoolSize() const
        {
            return m_poolSize;
        }

        virtual void SystemActivation(bool active)
        {
            m_isActive = active;
        }

        inline const String& GetName() const
        {
            return m_name;
        }

    protected:

        int         m_poolSize = 0;
        String m_name     = "";
        bool        m_isActive = false;
    };
} // namespace Lina::ECS

#endif
