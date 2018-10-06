/*
Author: Inan Evin
www.inanevin.com

BSD 2-Clause License
Lina Engine Copyright (c) 2018, Inan Evin All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.

-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO
-- THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
-- BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
-- GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
-- STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
-- OF SUCH DAMAGE.

4.0.30319.42000
10/6/2018 12:13:32 AM

*/

#pragma once

#ifndef Lina_Event_H
#define Lina_Event_H

#include <type_traits>
#include <tuple>
#include <SDL2/SDL_scancode.h>
#include <type_traits>
#include <functional>
#include <list>


// Alias of variadic function pointer, VFP.
template<typename ... Args>
using VFP = std::function<void(Args...)>;

// Alias of a shared pointer, that points to a list of VFPs.
template<typename ... Args>
using SPTR = std::shared_ptr<std::list<VFP<Args...>>>;

template <typename ... Args> struct Lina_Event : SPTR<Args...> {

	using EventHandler = std::function<void(Args...)>;
	using MyListeners = std::list<EventHandler>;

	Lina_Event() : std::shared_ptr<MyListeners>(std::make_shared<MyListeners>()) { }
	Lina_Event(const Lina_Event &) = delete;
	Lina_Event & operator=(const Lina_Event &) = delete;

	void SignalListeners(Args... args) 
	{
		for (auto &f : **this)
		{
			f(args...);
		}
	}

	struct Lina_Listener {

		std::weak_ptr<MyListeners> Event;
		typename MyListeners::iterator it;

		Lina_Listener() { }

		Lina_Listener(Lina_Event& event, EventHandler handler) 
		{
			ObserveEvent(event, handler);
		}

		Lina_Listener(Lina_Listener &&other) 
		{
			Event = other.Event;
			it = other.it;
			other.Event.reset();
		}

		Lina_Listener(const Lina_Listener &other) = delete;
		Lina_Listener & operator=(const Lina_Listener &other) = delete;

		Lina_Listener & operator=(Lina_Listener &&other)
		{
			Clear();
			Event = other.Event;
			it = other.it;
			other.Event.reset();
			return *this;
		}

		void ObserveEvent(Lina_Event& event, EventHandler handler) 
		{
			Clear();
			Event = event;
			it = event->insert(event->end(), handler);
		}

		void Clear()
		{
			if (!Event.expired()) Event.lock()->erase(it);
			Event.reset();
		}

		~Lina_Listener() { Clear(); }
	};


	Lina_Listener Connect(EventHandler h)
	{
		return Lina_Listener(*this, h);
	}


};





#endif