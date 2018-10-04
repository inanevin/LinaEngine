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
10/4/2018 1:00:21 AM

*/

#pragma once

#ifndef Lina_InputHandler_H
#define Lina_InputHandler_H
#include <algorithm>
#include <list>
#include <SDL2/SDL.h>



class Lina_InputHandler
{

public:

	enum EventType { OnKeyPress, OnKeyPressDelay, OnKeyPressPrev };

	struct BindedEvent {

	public:

		BindedEvent() {};
		~BindedEvent() {};
		BindedEvent(BindedEvent&& rhs) = default;	// Move constructor.

		SDL_Scancode m_Key;
		std::function<void()> Callback;
		Lina_InputHandler::EventType Type;

		std::string m_ID;


	private:

		BindedEvent(const BindedEvent& c) = delete;	// No copying allowed.
		friend bool operator==(const BindedEvent& lhs, const BindedEvent& rhs)
		{
			return lhs.m_ID == rhs.m_ID;
		}
	};


	std::list<BindedEvent> eventContainers;

	void BindMethod(EventType t, SDL_Scancode k, std::function<void()>&& callback, std::string id)
	{
		// Create a local event object.
		BindedEvent ev;
		ev.Type = t;
		ev.m_Key = k;
		ev.Callback = callback;
		ev.m_ID = id;

		// Use emplace back with move so that the local reference will be moved to memory location allocated for the list.
		// This will prevent unnecessary copies.
		eventContainers.emplace_back(std::move(ev));
	}
	
	void UnBindMethod(std::string id)
	{
		// Iterate through the list, if the matching id is found, remove it.
		// Note that this iteration will remove EVERY object that matches the id.
		std::list<BindedEvent>::iterator itri = eventContainers.begin();
		while (itri != eventContainers.end())
		{
			if (itri->m_ID == id)
			{
				itri = eventContainers.erase(itri++);
			}
			else
				++itri;
		}
	}

	void Update()
	{

	}

	void handleEvents(SDL_Event* e)
	{

		if (e->type == SDL_KEYDOWN)
		{

			for (std::list<BindedEvent>::iterator it = eventContainers.begin(); it != eventContainers.end(); it++)
			{
				if (it->m_Key == e->key.keysym.scancode)
				{
					it->Callback();
				}
			}
		}
	}
	Lina_InputHandler();

};


#endif