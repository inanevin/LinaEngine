#include "EventSystem/EventSystem.hpp"

#include "Log/Log.hpp"

namespace Lina::Event
{
    EventSystem* EventSystem::s_eventSystem = nullptr;

    void EventSystem::Initialize()
    {
        LINA_TRACE("[Initialization] -> Event System ({0})", typeid(*this).name());
    }

    void EventSystem::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> Event System ({0})", typeid(*this).name());
        m_mainDispatcher.clear();
    }
} // namespace Lina::Event
