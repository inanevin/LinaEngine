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
10/4/2018 1:31:18 AM

*/

#include "pch.h"
#include "Lina_InputBinding.h"  

// KEY PRESS METHOD CONSTRUCTOR
Lina_InputBinding::Lina_InputBinding(KeyPressEventType t, SDL_Scancode key, std::function<void()> && cb)
{	
	
	// Set event type params.
	m_KeyPressEventType = t;
	m_MousePressEventType = MousePressEventType::EMPTYMOUSEBUTTON;
	m_MouseMotionEventType = MouseMotionEventType::EMPTYMOUSEMOTION;
	
	// Set value params.
	m_Key = key;
	m_Mouse = MouseButton::MOUSE_EMPTY;
	m_KeyFrameLock = false;
	m_MouseFrameLock = false;

	// Increment statid id count.
	sNextID++;
	m_ID = sNextID;

	// Set binding type.
	m_BindingType = BindingType::KeyPress;

	// Set pointers.
	m_Callback = cb;
	//m_Bool = b;
}

// KEY PRESS BOOL CONSTRUCTOR
/*Lina_InputBinding::Lina_InputBinding(KeyPressEventType t, SDL_Scancode key, bool* b)
{
	// Set event type params.
	m_KeyPressEventType = t;
	m_MousePressEventType = MousePressEventType::EMPTYMOUSEBUTTON;
	m_MouseMotionEventType = MouseMotionEventType::EMPTYMOUSEMOTION;

	// Set value params.
	m_Key = key;
	m_Mouse = MouseButton::MOUSE_EMPTY;
	m_KeyFrameLock = false;
	m_MouseFrameLock = false;

	// Increment statid id count.
	sNextID++;
	m_ID = sNextID;

	// Set binding type.
	m_BindingType = BindingType::KeyBool;
}*/


// MOUSE PRESS METHOD CONSTRUCTOR
Lina_InputBinding::Lina_InputBinding(MousePressEventType t, MouseButton but, std::function<void()> && cb)
{
	// Set event type params.
	m_KeyPressEventType = KeyPressEventType::EMPTYKEY;
	m_MousePressEventType = t;
	m_MouseMotionEventType = MouseMotionEventType::EMPTYMOUSEMOTION;

	// Set value params.
	m_Mouse = but;
	m_KeyFrameLock = false;
	m_MouseFrameLock = false;

	// Increment statid id count.
	sNextID++;
	m_ID = sNextID;

	// Set binding type.
	m_BindingType = BindingType::MousePress;

	// Set pointers.
	m_Callback = cb;
	//m_Bool = b;

}

// MOUSE PRESS BOOL CONSTRUCTOR
/*Lina_InputBinding::Lina_InputBinding(MousePressEventType t, MouseButton but, bool* b)
{
	// Set event type params.
	m_KeyPressEventType = KeyPressEventType::EMPTYKEY;
	m_MousePressEventType = t;
	m_MouseMotionEventType = MouseMotionEventType::EMPTYMOUSEMOTION;

	// Set value params.
	m_Mouse = but;
	m_KeyFrameLock = false;
	m_MouseFrameLock = false;

	// Increment statid id count.
	sNextID++;
	m_ID = sNextID;

	// Set binding type.
	m_BindingType = BindingType::Mouse;
}
*/

// MOUSE MOTION CONSTRUCTOR
Lina_InputBinding::Lina_InputBinding(MouseMotionEventType t, std::weak_ptr<float> f)
{
	m_KeyPressEventType = KeyPressEventType::EMPTYKEY;
	m_MousePressEventType = MousePressEventType::EMPTYMOUSEBUTTON;
	m_MouseMotionEventType = t;

	// Set value params.
	m_Mouse = MouseButton::MOUSE_EMPTY;
	m_KeyFrameLock = false;
	m_MouseFrameLock = false;

	// Increment statid id count.
	sNextID++;
	m_ID = sNextID;

	// Set binding type.
	m_BindingType = BindingType::MouseMotion;

	// Set pointers.
	m_Float = f;
	m_Callback = NULL;
}


