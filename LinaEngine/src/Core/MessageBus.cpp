#include "Core/MessageBus.hpp"

#include "EventSystem/EventSystem.hpp"

namespace Lina
{
    void MessageBus::Initialize(ApplicationMode appMode)
    {
        auto* eventSystem = Event::EventSystem::Get();
        m_appMode         = appMode;
    }
} // namespace Lina
