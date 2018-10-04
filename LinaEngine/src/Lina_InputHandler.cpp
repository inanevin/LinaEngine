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
10/4/2018 1:00:34 AM

*/

#include "pch.h"
#include "Lina_InputHandler.h"  

void Lina_InputHandler::HandleEvents(SDL_Event& e)
{
	for (std::list<Lina_InputBinding>::iterator it = keyPressEventContainers.begin(); it != keyPressEventContainers.end(); it++)
	{
		// KEYBOARD EVENTS
		if (e.type == SDL_KEYDOWN)
		{
			if (it->m_EventType == InputEventType::OnKey)
			{
				if (it->m_Key == e.key.keysym.scancode)
				{
					it->m_Callback();	// execute callback if event & key matches.
				}
			}
			else if (it->m_EventType == InputEventType::OnKeyDown)
			{
				// Execute only if the frame lock is not present for that particular binding.
				if (!it->m_KeyFrameLock && it->m_Key == e.key.keysym.scancode)
				{
					// Lock the particular binding to prevent it being called on next frame.
					it->m_KeyFrameLock = true;
					it->m_Callback();
				}
			}
		}
		if (e.type == SDL_KEYUP)
		{
			// Remove a frame lock if a key that was pressed have been released.
			if (it->m_KeyFrameLock && it->m_Key == e.key.keysym.scancode)
				it->m_KeyFrameLock = false;

			// Execute if type and key matches.
			if (it->m_EventType == InputEventType::OnKeyUp)
			{
				if (it->m_Key == e.key.keysym.scancode)
					it->m_Callback();
			}
		}
	
		// Mouse Events
		if (e.type == SDL_MOUSEBUTTONDOWN)
		{
			if (it->m_EventType == InputEventType::OnMouseButton)
			{
				// Fire off events if respective buttons are passed.
				if (it->m_Mouse == e.button.button)
				{
					it->m_Callback();

					// SDL mouse events work differently than keyboard events, they only get caught a single frame when they were pressed.
					// So in order to work with OnMouseButton event, which means holding down the button, we release the mouseLock to call
					// the respective event until the lock has been captured, by an OnMouseButtonUp event.
					if (!it->m_MouseFrameLock)
						it->m_MouseFrameLock = true;
				}
			}
			else if (it->m_EventType == InputEventType::OnMouseButtonDown)
			{
				// Fire off events if respective buttons are passed.
				if (it->m_Mouse == e.button.button)
					it->m_Callback();
			}
		}

		if (e.type == SDL_MOUSEBUTTONUP)
		{
			// Lock the frame lock if it was released previously.
			if (it->m_MouseFrameLock && it->m_Mouse == e.button.button)
				it->m_MouseFrameLock = false;

			if (it->m_EventType == InputEventType::OnMouseButtonUp)
			{
				// Fire off events if respective buttons are passed.
				if (it->m_Mouse == e.button.button)
					it->m_Callback();
			}
		}

	}

}

void Lina_InputHandler::Bind(Lina_InputBinding& binding)
{
	// Add the binding with emplace_back and std::move so we will avoid copying, we will use the move constructor.

	if(binding.m_EventType == InputEventType::OnKey || binding.m_EventType == InputEventType::OnKeyDown || binding.m_EventType == InputEventType::OnKeyUp)
		keyPressEventContainers.emplace_back(std::move(binding));
	else if(binding.m_EventType == InputEventType::OnMouseButton || binding.m_EventType == InputEventType::OnMouseButtonDown || binding.m_EventType == InputEventType::OnMouseButtonUp)
		mousePressEventContainers.emplace_back(std::move(binding));
}

void Lina_InputHandler::Unbind(Lina_InputBinding& binding)
{
	// Iterate through the list, if the matching id is found, remove it.
	// Note that this iteration will remove EVERY object that matches the id, so unique usage of the static ID counter must be cared about.
	std::list<Lina_InputBinding>::iterator itri = keyPressEventContainers.begin();
	while (itri != keyPressEventContainers.end())
	{
		if (*itri == binding)
		{
			itri = keyPressEventContainers.erase(itri++);
		}
		else
			++itri;
	}
}

void Lina_InputHandler::Update()
{
	SDL_PumpEvents();

	for (std::list<Lina_InputBinding>::iterator it = mousePressEventContainers.begin(); it != mousePressEventContainers.end(); it++)
	{
		// Check the frame locked bindings first.
		if (it->m_MouseFrameLock)
		{
			// If a binding is frame locked, it means the corresponding button was pressed before
			// Now if the corresponding button is not pressed this frame, it means it was released.
			// If the event type is OnMouseButtonDown, then simply unlock the frame lock, the rest will be done by
			// other control blocks. However if the event type is mouse up, unlock the frame lock, then call the corresponding
			// event since we want the event to be fired on the frame when the button was released, which is this frame.
			if (it->m_Mouse == MouseButton::MOUSE_LEFT && !(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
			{
				it->m_MouseFrameLock = false;

				if (it->m_EventType == InputEventType::OnMouseButtonUp)
					it->m_Callback();
			}
			else if (it->m_Mouse == MouseButton::MOUSE_MIDDLE && !(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE)))
			{
				it->m_MouseFrameLock = false;
				if (it->m_EventType == InputEventType::OnMouseButtonUp)
					it->m_Callback();
			}
			else if (it->m_Mouse == MouseButton::MOUSE_RIGHT && !(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)))
			{
				it->m_MouseFrameLock = false;

				if (it->m_EventType == InputEventType::OnMouseButtonUp)
					it->m_Callback();
			}
		}

		// Check the OnMouseButton events and fire the callbacks each frame the buttons are held.
		if (it->m_EventType == InputEventType::OnMouseButton)
		{
			if (!it->m_MouseFrameLock)
			{
				if (it->m_Mouse == MouseButton::MOUSE_LEFT && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
					it->m_Callback();
				else if (it->m_Mouse == MouseButton::MOUSE_MIDDLE && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE))
					it->m_Callback();
				else if (it->m_Mouse == MouseButton::MOUSE_RIGHT && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))
					it->m_Callback();
			}
		}
		// Check the OnMouseButtonDown events, and fire the callbacks in this frame, then frame lock the bindings, so that they
		// won't be called until they are released and pressed again.
		if (it->m_EventType == InputEventType::OnMouseButtonDown)
		{
			if (!it->m_MouseFrameLock)
			{
				if (it->m_Mouse == MouseButton::MOUSE_LEFT && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
				{
					it->m_Callback();
					it->m_MouseFrameLock = true;
				}
				else if (it->m_Mouse == MouseButton::MOUSE_MIDDLE && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE))
				{
					it->m_Callback();
					it->m_MouseFrameLock = true;
				}
				else if (it->m_Mouse == MouseButton::MOUSE_RIGHT &&  SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))
				{
					it->m_Callback();
					it->m_MouseFrameLock = true;
				}
			}
		}

		// If a binding which is registered as button up and is pressed currently, frame lock it and prepare it for the
		// control body of the future frames to fire it.
		if (it->m_EventType == InputEventType::OnMouseButtonUp)
		{
			if (it->m_Mouse == MouseButton::MOUSE_LEFT && (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
			{
				it->m_MouseFrameLock = true;
			}
			else if (it->m_Mouse == MouseButton::MOUSE_LEFT && (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
			{
				it->m_MouseFrameLock = true;
			}
			else if (it->m_Mouse == MouseButton::MOUSE_LEFT && (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
			{
				it->m_MouseFrameLock = true;
			}
		}

	}
	
}

