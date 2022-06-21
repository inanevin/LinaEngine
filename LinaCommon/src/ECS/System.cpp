#include "ECS/System.hpp"
#include "ECS/Registry.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/ECSEvents.hpp"

namespace Lina::ECS
{
    void System::Initialize(const String& name)
    {
        m_name = name;
    }
} // namespace Lina::ECS
