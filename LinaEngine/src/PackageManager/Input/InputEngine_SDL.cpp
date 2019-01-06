/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Ýnan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: InputEngine_SDL
Timestamp: 1/6/2019 2:18:10 AM

*/

#include "LinaPch.hpp"
#include "InputEngine_SDL.hpp" 


namespace LinaEngine
{

#define MOUSE_ACCURACY	10
#define MOUSE_SMOOTH 25


	static bool isSDLInputInitialized = false;

	void InputEngine_SDL::SDLErrorCallback(const char* description)
	{
		LINA_CORE_ERR("SDL Error: {1}", description);
	}

	InputEngine_SDL::InputEngine_SDL()
	{

		// Initialize the array.
		m_PreviousKeys = new Uint8[numKeys];

		// Store the key state into member attribute.
		m_KeyboardState = SDL_GetKeyboardState(&numKeys);

		// Allocate arrays to store previous and current keys, set them to current key state initially.
		m_PreviousKeys = new Uint8[numKeys];
		m_CurrentKeys = new Uint8[numKeys];
		memcpy(m_CurrentKeys, m_KeyboardState, sizeof(Uint8) * numKeys);
		memcpy(m_PreviousKeys, m_KeyboardState, sizeof(Uint8) * numKeys);


		if (!isSDLInputInitialized)
		{
			int status = SDL_Init(SDL_INIT_EVENTS);
			if (status != 0) SDLErrorCallback(SDL_GetError());
			LINA_CORE_ASSERT(status == 0, "Could not initialize SDL input!");
			isSDLInputInitialized = true;
		}


	}

	InputEngine_SDL::~InputEngine_SDL()
	{
		delete m_PreviousKeys;
		delete m_CurrentKeys;
	}

	void InputEngine_SDL::OnUpdate()
	{
		// Store current mouse coordinates.
		SDL_GetRelativeMouseState(&mouseXState, &mouseYState);

		// Update mouse x & y state into float variables.
		currentMouseX = mouseXState;
		currentMouseY = mouseYState;

		// Divide the current values by an accuracy value to create precision.
		deltaMouseX = float((currentMouseX) / MOUSE_ACCURACY);
		deltaMouseX = float((currentMouseX) / MOUSE_ACCURACY);

		// Interpolate smoothed mouse input. TODO: Interpolate smoothing later.
		smoothDeltaMouseX = deltaMouseX;
		smoothDeltaMouseY = deltaMouseY;
		//smoothDeltaMouseX = Lina_Math::Lerp(smoothDeltaMouseX, deltaMouseX, Lina_Time::GetDelta() * MOUSE_SMOOTH);
		//smoothDeltaMouseY = Lina_Math::Lerp(smoothDeltaMouseY, deltaMouseY, Lina_Time::GetDelta() * MOUSE_SMOOTH);

		// Store the previous keys.
		memcpy(m_PreviousKeys, m_CurrentKeys, sizeof(Uint8) * numKeys);

		// Update the current keys, copy the current keys to pressed keys.
		memcpy(m_CurrentKeys, m_KeyboardState, sizeof(Uint8) * numKeys);

		// Store previous mouse states.
		mouse0Previous = mouse0Current;
		mouse1Previous = mouse1Current;
		mouse2Previous = mouse2Current;

		// Update current mouse states.
		mouse0Current = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT);
		mouse1Current = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT);
		mouse2Current = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE);
	}

}

