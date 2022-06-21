/*
Class: SystemList



Timestamp: 12/25/2021 12:35:15 PM
*/

#pragma once

#ifndef SystemList_HPP
#define SystemList_HPP

// Headers here.
#include "Data/Vector.hpp"

namespace Lina::ECS
{
    class System;

    class SystemList
    {
    public:
        SystemList()  = default;
        ~SystemList() = default;

        bool AddSystem(System& system);
        void UpdateSystems(float delta);
        bool RemoveSystem(System& system);

        inline const Vector<System*>& GetSystems() const
        {
            return m_systems;
        }

    private:
        Vector<System*> m_systems;
    };
} // namespace Lina::ECS

#endif
