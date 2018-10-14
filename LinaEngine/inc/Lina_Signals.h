/*
Author: Inan Evin
www.inanevin.com

MIT License

Lina Engine, Copyright (c) 2018 Inan Evin

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

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