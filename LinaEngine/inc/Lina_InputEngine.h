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
10/9/2018 3:02:19 AM

*/

#pragma once

#ifndef Lina_InputEngine_H
#define Lina_InputEngine_H

#include <Lina_Actions.h>
#include <Lina_SDLHandler.h>

class Lina_InputEngine
{

public:

	Lina_InputEngine() {};
	~Lina_InputEngine();
	void Initialize();
	void Update();
	void HandleEvents(SDL_Event& e);
	float GetRawMouseX();
	float GetRawMouseY();
	float GetMouseX();
	float GetMouseY();
	bool GetKey(SDL_Scancode);
	bool GetKeyDown(SDL_Scancode);
	bool GetKeyUp(SDL_Scancode);
	bool GetMouse(int);
	bool GetMouseDown(int);
	bool GetMouseUp(int);

	Lina_ActionDispatcher* GetInputDispatcher() { return &m_InputDispatcher; }

private:

	void operator=(const Lina_InputEngine& r) = delete;
	Lina_InputEngine(const Lina_InputEngine& r) = delete;
	Lina_ActionDispatcher m_InputDispatcher;

	const Uint8* m_KeyboardState;
	Uint8* m_PreviousKeys;
	Uint8* m_CurrentKeys;
	
	int numKeys = 256;
	float deltaMouseX;
	float deltaMouseY;
	float smoothDeltaMouseX;
	float smoothDeltaMouseY;
	float currentMouseX;
	float currentMouseY;
	int mouseXState;
	int mouseYState;
	bool mouse0Previous;
	bool mouse1Previous;
	bool mouse2Previous;
	bool mouse0Current;
	bool mouse1Current;
	bool mouse2Current;
};


#endif