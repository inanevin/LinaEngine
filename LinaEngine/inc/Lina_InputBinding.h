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
#include "Lina_Console.h"

static int sNextKeyBindingID = 0;
static int sNextMouseBindingID = 0;

enum KeyPressEventType {
	OnKey,
	OnKeyDown,
	OnKeyUp
};

enum MousePressEventType {
	OnMouseButton,
	OnMouseButtonDown,
	OnMouseButtonUp
};

enum MouseButton {
	MOUSE_LEFT = 1,
	MOUSE_MIDDLE = 2,
	MOUSE_RIGHT = 3
};

class Lina_KeyBinding
{
public:

	// Constructors

	Lina_KeyBinding(KeyPressEventType t, SDL_Scancode key, std::function<void()> && cb) : m_EventType(t), m_Key(key), m_Callback(cb)
	{
		m_KeyFrameLock = false;
		sNextKeyBindingID++;
		m_ID = sNextKeyBindingID;
	};

	~Lina_KeyBinding() {};
	Lina_KeyBinding(Lina_KeyBinding&& rhs) = default;

	// Attributes

	SDL_Scancode m_Key;
	std::function<void()> m_Callback;
	KeyPressEventType m_EventType;
	int m_ID;
	bool m_KeyFrameLock;

private:

	Lina_KeyBinding(const Lina_KeyBinding& c) = delete;
	// Comparison operator override for comparing IDs.
	friend bool operator==(const Lina_KeyBinding& lhs, const Lina_KeyBinding& rhs)
	{
		return lhs.m_ID == rhs.m_ID;
	}
};

class Lina_MouseBinding
{

public:

	// Constructors
	Lina_MouseBinding(MousePressEventType t, MouseButton but, std::function<void()> && cb) : m_EventType(t), m_Mouse(but), m_Callback(cb)
	{
		m_MouseFrameLock = false;
		sNextMouseBindingID++;
		m_ID = sNextMouseBindingID;
	};
	~Lina_MouseBinding() {};
	Lina_MouseBinding(Lina_MouseBinding&& rhs) = default;

	// Data attributes

	int m_Mouse;
	std::function<void()> m_Callback;
	MousePressEventType m_EventType;
	int m_ID;
	bool m_MouseFrameLock;

private:

	Lina_MouseBinding(const Lina_MouseBinding& c) = delete;
	// Comparison operator override for comparing IDs.
	friend bool operator==(const Lina_MouseBinding& lhs, const Lina_MouseBinding& rhs)
	{
		return lhs.m_ID == rhs.m_ID;
	}

};


#endif