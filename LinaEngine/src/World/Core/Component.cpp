/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "World/Core/Component.hpp"
#include "World/Core/World.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include "EventSystem/ComponentEvents.hpp"

namespace Lina::World
{
    void Component::SaveToArchive(Serialization::Archive<OStream>& oarchive)
    {
        oarchive(m_entityID);
    }
    void Component::LoadFromArchive(Serialization::Archive<IStream>& iarchive)
    {
        iarchive(m_entityID);
    }
    void Component::OnComponentCreated()
    {
        auto* es = Event::EventSystem::Get();

        es->Trigger<Event::EComponentCreated>({this, GetTID()});

        if (GetComponentMask().IsSet(ComponentMask::ReceiveOnGameEnd))
            es->Connect<Event::EEndGame, &Component::OnEndGame>(this);

        if (GetComponentMask().IsSet(ComponentMask::ReceiveOnGameStart))
            es->Connect<Event::EStartGame, &Component::OnStartGame>(this);

        if (GetComponentMask().IsSet(ComponentMask::ReceiveOnPostPhysicsTick))
            es->Connect<Event::EPostPhysicsTick, &Component::OnPostPhysicsTick>(this);

        if (GetComponentMask().IsSet(ComponentMask::ReceiveOnPostTick))
            es->Connect<Event::EPostTick, &Component::OnPostTick>(this);

        if (GetComponentMask().IsSet(ComponentMask::ReceiveOnPreTick))
            es->Connect<Event::EPreTick, &Component::OnPreTick>(this);

        if (GetComponentMask().IsSet(ComponentMask::ReceiveOnTick))
            es->Connect<Event::ETick, &Component::OnTick>(this);
    }

    void Component::OnComponentDestroyed()
    {
        auto* es = Event::EventSystem::Get();

        es->Trigger<Event::EComponentDestroyed>({this, GetTID()});

        if (GetComponentMask().IsSet(ComponentMask::ReceiveOnGameEnd))
            es->Disconnect<Event::EEndGame>(this);

        if (GetComponentMask().IsSet(ComponentMask::ReceiveOnGameStart))
            es->Disconnect<Event::EStartGame>(this);

        if (GetComponentMask().IsSet(ComponentMask::ReceiveOnPostPhysicsTick))
            es->Disconnect<Event::EPostPhysicsTick>(this);

        if (GetComponentMask().IsSet(ComponentMask::ReceiveOnPostTick))
            es->Disconnect<Event::EPostTick>(this);

        if (GetComponentMask().IsSet(ComponentMask::ReceiveOnPreTick))
            es->Disconnect<Event::EPreTick>(this);

        if (GetComponentMask().IsSet(ComponentMask::ReceiveOnTick))
            es->Disconnect<Event::ETick>(this);
    }
} // namespace Lina::World
