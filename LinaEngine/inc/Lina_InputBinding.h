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



static int sNextID = 0;


enum MouseButton {
	MOUSE_LEFT = 1,
	MOUSE_MIDDLE = 2,
	MOUSE_RIGHT = 3,
	MOUSE_EMPTY = -1
};

enum KeyPressEventType {
	OnKey,
	OnKeyDown,
	OnKeyUp,
	EMPTYKEY
};

enum MousePressEventType {

	OnMouseButton,
	OnMouseButtonDown,
	OnMouseButtonUp,
	EMPTYMOUSEBUTTON
};

enum MouseMotionEventType {
	MouseMotionX,
	MouseMotionY,
	EMPTYMOUSEMOTION
};

enum BindingType {
	KeyPress,
	MousePress,
	MouseMotion
};

class Lina_InputBinding
{

public:


	// Param constructor for keyboard binding to a method.
	Lina_InputBinding(KeyPressEventType, SDL_Scancode, std::function<void()> &&);

	// Param constructor for keyboard binding to a boolean.
	//Lina_InputBinding(KeyPressEventType t, SDL_Scancode key, bool* b);

	// Param constructor for mouse binding to a method.
	Lina_InputBinding(MousePressEventType, MouseButton, std::function<void()> &&) ;

	// Param constructor for mouse binding to a boolean.
	//Lina_InputBinding(MousePressEventType t, MouseButton but, bool* b);

	// Param constructor for mouse motion binding to a float.
	Lina_InputBinding(MouseMotionEventType, std::weak_ptr<float>);

	~Lina_InputBinding() {};
	Lina_InputBinding(Lina_InputBinding&& rhs) = default;

	friend class Lina_InputHandler;

private:

	// No copy constructor is allowed.
	Lina_InputBinding(const Lina_InputBinding& c) = delete;

	// Comparison operator override for comparing IDs.
	friend bool operator==(const Lina_InputBinding& lhs, const Lina_InputBinding& rhs)
	{ return lhs.m_ID == rhs.m_ID; }


	// ATTRIBUTES

	std::function<void()> m_Callback;

	MousePressEventType m_MousePressEventType;
	KeyPressEventType m_KeyPressEventType;
	MouseMotionEventType m_MouseMotionEventType;
	BindingType m_BindingType;

	SDL_Scancode m_Key;
	bool m_KeyFrameLock;
	bool m_MouseFrameLock;
	std::weak_ptr<bool> m_Bool;
	std::weak_ptr<float> m_Float;
	int m_Mouse;
	int m_ID;
};


#endif