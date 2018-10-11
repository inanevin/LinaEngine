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
10/9/2018 3:02:34 AM

*/

#include "pch.h"
#include "Lina_InputEngine.h"  
#include "Lina_CoreMessageBus.h"

Lina_InputEngine::Lina_InputEngine()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Input Engine Initialized.", Lina_Console::MsgType::Initialization, "Input Engine");

	// Provide the engine & input dispatcher to message bus.
	Lina_CoreMessageBus::Instance().SetInputEngine(this);
	Lina_CoreMessageBus::Instance().SetInputDispatcher(&m_InputDispatcher);
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
	if (e.type == SDL_KEYUP)
	{
		Lina_Action<SDL_Scancode> keyReleased = Lina_Action<SDL_Scancode>(KeyReleased);
		keyReleased.SetData(e.key.keysym.scancode);
		m_InputDispatcher.DispatchAction(keyReleased);
	}
	if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		Lina_Action<int> mouseButtonDown = Lina_Action<int>(MouseButtonPressed);
		mouseButtonDown.SetData(e.button.button);
		m_InputDispatcher.DispatchAction(mouseButtonDown);
	}
	if (e.type == SDL_MOUSEBUTTONUP)
	{
		Lina_Action<int> mouseButtonUp = Lina_Action<int>(MouseButtonReleased);
		mouseButtonUp.SetData(e.button.button);
		m_InputDispatcher.DispatchAction(mouseButtonUp);
	}
	if (e.type == SDL_MOUSEMOTION)
	{
		mouseXT = true;
		prevMouseX = currentMouseX;
		currentMouseX = e.motion.x;
		relativeMouseX = currentMouseX - prevMouseX;
	}
	mouseXT = false;
}
void Lina_InputEngine::Update()
{
	if (mouseXT)
	{
		mouseXT = false;
		//std::cout << "aq";
		relativeMouseX = 0;
	}
	/*prevMouseX = currentMouseX;
	prevMouseY = currentMouseY;

	SDL_PumpEvents();


	SDL_GetMouseState(&currentMouseX, &currentMouseY);*/
}

float Lina_InputEngine::GetMouseAxis(int axis)
{
	if (axis ==  0)
	{

		return relativeMouseX;
	}
}

