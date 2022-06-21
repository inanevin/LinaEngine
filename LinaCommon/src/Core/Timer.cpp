#include "Core/Timer.hpp"

#include "Log/Log.hpp"

namespace Lina
{
    HashMap<String, Timer*> Timer::s_activeTimers;

    void Timer::Stop()
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli>                   ms  = now - m_startTimePoint;
        m_duration                                                      = ms.count();
        m_active                                                        = false;
    }

    Timer& Timer::GetTimer(const String& name)
    {
        if (s_activeTimers.find(name) == s_activeTimers.end())
            s_activeTimers[name] = new Timer();

        return *s_activeTimers[name];
    }

    void Timer::UnloadTimers()
    {
        for (HashMap<String, Timer*>::iterator it = s_activeTimers.begin(); it != s_activeTimers.end(); ++it)
            delete it->second;

        s_activeTimers.clear();
    }
} // namespace Lina
