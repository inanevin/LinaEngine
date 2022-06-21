#include "Log/Log.hpp"

#include <sstream>

namespace Lina
{
    Event::Signal<void(const Event::ELog&)> Log::s_onLog;
}
