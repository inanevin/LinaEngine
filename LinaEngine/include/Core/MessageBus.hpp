/*
Class: MessageBus



Timestamp: 12/24/2021 10:43:34 AM
*/

#pragma once

#ifndef MessageBus_HPP
#define MessageBus_HPP

// Headers here.
#include "Core/CommonApplication.hpp"

namespace Lina
{
    class Engine;

    class MessageBus
    {

    public:
        MessageBus() = default;
        ~MessageBus() = default;

    private:
        friend class Engine;

        void Initialize(ApplicationMode appMode);

    private:
        ApplicationMode m_appMode = ApplicationMode::Editor;
    };
} // namespace Lina

#endif
