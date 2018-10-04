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
10/4/2018 1:31:07 AM

*/

#pragma once

#ifndef Lina_InputBinding_H
#define Lina_InputBinding_H

#include <SDL2/SDL.h>

static int sNextID = 0;

class Lina_InputBinding
{

public:

	enum EventType { OnKeyPress, OnKeyPressDelay, OnKeyPressPrev };

	// Param constructor, increment statid ID counter.
	Lina_InputBinding(EventType t, SDL_Scancode key, std::function<void()> && cb) : m_Type(t), m_Key(key), m_Callback(cb) {
		sNextID++;
		m_ID = sNextID;
	};

	~Lina_InputBinding() {};
	Lina_InputBinding(Lina_InputBinding&& rhs) = default;
	SDL_Scancode m_Key;
	std::function<void()> m_Callback;
	EventType m_Type;
	int m_ID;

private:

	Lina_InputBinding(const Lina_InputBinding& c) = delete;

	// Comparison operator override for comparing IDs.
	friend bool operator==(const Lina_InputBinding& lhs, const Lina_InputBinding& rhs)
	{
		return lhs.m_ID == rhs.m_ID;
	}
};


#endif