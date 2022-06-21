/*
Class: EventCommon



Timestamp: 12/25/2020 11:54:49 PM
*/

#pragma once

#ifndef EventCommon_HPP
#define EventCommon_HPP

// Headers here.

#define ENTT_USE_ATOMIC
#include <entt/signal/delegate.hpp>
#include <entt/signal/dispatcher.hpp>
#include <entt/signal/fwd.hpp>
#include <entt/signal/sigh.hpp>

namespace Lina::Event
{
    template <typename T> using Signal = entt::sigh<T>;

    template <typename T> using Sink = entt::sink<T>;

    using Dispatcher = entt::dispatcher;

    template <typename T> using Delegate = entt::delegate<T>;

} // namespace Lina::Event

#endif
