#include "ECS/SystemList.hpp"
#include "ECS/System.hpp"

namespace Lina::ECS
{
    bool SystemList::AddSystem(System& system)
    {
        m_systems.push_back(&system);
        return true;
    }

    void SystemList::UpdateSystems(float delta)
    {
        for (auto s : m_systems)
            s->UpdateComponents(delta);
    }

    bool SystemList::RemoveSystem(System& system)
    {
        for (unsigned int i = 0; i < m_systems.size(); i++)
        {
            // If the addr of the target system matches any system, erase it from the array.
            if (&system == m_systems[i])
            {
                m_systems.erase(m_systems.begin() + i);
                return true;
            }
        }

        return false;
    }

} // namespace Lina::ECS
