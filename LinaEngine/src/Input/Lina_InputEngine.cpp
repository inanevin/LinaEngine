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
10/9/2018 3:02:34 AM

*/

#include "pch.h"
#include "Input/Lina_InputEngine.h"  
#include "Lina_CoreMessageBus.h"
#include "Math/Lina_Math.h"
#include "Utility/Lina_Time.h"

#define MOUSE_ACCURACY	10
#define MOUSE_SMOOTH 25


void Lina_InputEngine::Initialize()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Input Engine Initialized.", Lina_Console::MsgType::Initialization, "Input Engine");

	// Provide the engine & input dispatcher to message bus.
	Lina_CoreMessageBus::Instance().SetInputEngine(this);

	// Initialize the array.
	m_PreviousKeys = new Uint8[numKeys];

	// Store the key state into member attribute.
	m_KeyboardState = SDL_GetKeyboardState(&numKeys);

	// Allocate arrays to store previous and current keys, set them to current key state initially.
	m_PreviousKeys = new Uint8[numKeys];
	m_CurrentKeys = new Uint8[numKeys];
	memcpy(m_CurrentKeys, m_KeyboardState, sizeof(Uint8) * numKeys);
	memcpy(m_PreviousKeys, m_KeyboardState, sizeof(Uint8) * numKeys);

}

Lina_InputEngine::~Lina_InputEngine()
{
	delete m_PreviousKeys;
	delete m_CurrentKeys;
}

void Lina_InputEngine::HandleEvents(SDL_Event& e)
{
	// Set the data for the actions and dispatch them.
	if (e.type == SDL_KEYDOWN)
	{
		Lina_Action<SDL_Scancode> keyPressed = Lina_Action<SDL_Scancode>(KeyPressed);
		keyPressed.SetData(e.key.keysym.scancode);
		m_InputDispatcher.DispatchAction(keyPressed);
	}
	else if (e.type == SDL_KEYUP)
	{
		Lina_Action<SDL_Scancode> keyReleased = Lina_Action<SDL_Scancode>(KeyReleased);
		keyReleased.SetData(e.key.keysym.scancode);
		m_InputDispatcher.DispatchAction(keyReleased);
	}
	else if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		Lina_Action<int> mouseButtonDown = Lina_Action<int>(MouseButtonPressed);
		mouseButtonDown.SetData(e.button.button);
		m_InputDispatcher.DispatchAction(mouseButtonDown);
	}
	else if (e.type == SDL_MOUSEBUTTONUP)
	{
		Lina_Action<int> mouseButtonUp = Lina_Action<int>(MouseButtonReleased);
		mouseButtonUp.SetData(e.button.button);
		m_InputDispatcher.DispatchAction(mouseButtonUp);
	}
	else if (e.type == SDL_QUIT)
	{
		Lina_Action<> sdlQuit = Lina_Action<>(SDLQuit);
		m_InputDispatcher.DispatchAction(sdlQuit);
	}

}

void Lina_InputEngine::Update()
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
	m_PreviousKeys = new Uint8[numKeys];
	memcpy(m_PreviousKeys, m_CurrentKeys, sizeof(Uint8) * numKeys);

	// Update the current keys, copy the current keys to pressed keys.
	m_CurrentKeys = new Uint8[numKeys];
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

float Lina_InputEngine::GetRawMouseX()
{
	return deltaMouseX;
}

float Lina_InputEngine::GetRawMouseY()
{
	return deltaMouseY;
}

float Lina_InputEngine::GetMouseX()
{
	return smoothDeltaMouseX;
}

float Lina_InputEngine::GetMouseY()
{
	return smoothDeltaMouseY;
}

bool Lina_InputEngine::GetKey(SDL_Scancode sc)
{
	int index = (int)sc;
	return *(m_CurrentKeys + index);
}

bool Lina_InputEngine::GetKeyUp(SDL_Scancode sc)
{
	int index = (int)sc;
	return (*(m_PreviousKeys + index) && !(*(m_CurrentKeys + index)));
}

bool Lina_InputEngine::GetKeyDown(SDL_Scancode sc)
{
	int index = (int)sc;
	return (!(*(m_PreviousKeys + index)) && (*(m_CurrentKeys + index)));
}

bool Lina_InputEngine::GetMouseButton(int mouse)
{
	if (mouse == 0)
		return mouse0Current;
	else if (mouse == 1)
		return mouse1Current;
	else if (mouse == 2)
		return mouse2Current;
}

bool Lina_InputEngine::GetMouseButtonDown(int mouse)
{
	if (mouse == 0)
		return !mouse0Previous && mouse0Current;
	else if (mouse == 1)
		return !mouse1Previous && mouse1Current;
	else if (mouse == 2)
		return !mouse2Previous && mouse2Current;
}

bool Lina_InputEngine::GetMouseButtonUp(int mouse)
{
	if (mouse == 0)
		return mouse0Previous && !mouse0Current;
	else if (mouse == 1)
		return mouse1Previous && !mouse1Current;
	else if (mouse == 2)
		return mouse2Previous && !mouse2Current;
}