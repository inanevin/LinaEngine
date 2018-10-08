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

template <typename ... Args> struct Lina_Signal : SPTR<Args...> {

	using SignalHandler = std::function<void(Args...)>;
	using Slots = std::list<SignalHandler>;

	Lina_Signal() : std::shared_ptr<Slots>(std::make_shared<Slots>()) { }
	Lina_Signal(const Lina_Signal &) = delete;
	Lina_Signal & operator=(const Lina_Signal &) = delete;

	// Init callbacks on listeners.
	void SignalListeners(Args... args) 
	{
		for (auto &f : **this)
		{
			f(args...);
		}
	}

	struct Lina_Listener {

		std::weak_ptr<Slots> slot;
		typename Slots::iterator it;

		Lina_Listener() { }

		Lina_Listener(Lina_Signal& event, SignalHandler handler) 
		{
			ObserveSignal(event, handler);
		}

		Lina_Listener(Lina_Listener &&other) 
		{
			slot = other.slot;
			it = other.it;
			other.slot.reset();
		}

		Lina_Listener(const Lina_Listener &other) = delete;
		Lina_Listener & operator=(const Lina_Listener &other) = delete;

		Lina_Listener & operator=(Lina_Listener &&other)
		{
			Clear();
			slot = other.slot;
			it = other.it;
			other.slot.reset();
			return *this;
		}

		void ObserveSignal(Lina_Signal& event, SignalHandler handler) 
		{
			Clear();
			slot = event;
			it = event->insert(event->end(), handler);
		}

		void Clear()
		{
			if (!slot.expired()) slot.lock()->erase(it);
			slot.reset();
		}

		~Lina_Listener() { Clear(); }
	};


	Lina_Listener Connect(SignalHandler h)
	{
		return Lina_Listener(*this, h);
	}


};


/* EXAMPLE SIGNALS 
Lina_Signal<SDL_Scancode> onKey;
Lina_Signal<SDL_Scancode> onKeyDown;
Lina_Signal<SDL_Scancode> onKeyUp;
Lina_Signal<int> onMouse;
Lina_Signal<int> onMouseDown;
Lina_Signal<float, float> onMouseMotion;*/

class Lina_SignalDispatcher
{
public:

	template<typename ... Args>
	void DispatchSignal(Lina_Signal<Args...>& ev, Args ... args)
	{
		ev.SignalListeners(args...);
	}

};


#endif