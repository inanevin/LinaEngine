
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
10/2/2018 11:11:56 AM

*/


#pragma once

#ifndef Lina_Input_H
#define Lina_Input_H


#include <SDL2/SDL.h>
#include <vector>


enum MouseButtons { LEFT = 0, RIGHT = 1, MIDDLE = 2 };

class Lina_InputManager
{
public:
	Lina_InputManager();
	~Lina_InputManager();

	void Update();
	bool GetKeyDown(SDL_Scancode) const;
	bool GetMouseButtonDown(int) const;

private:
	Uint8* m_Keys;
	Uint8* m_PressedKeys;
	


	void OnKeyDown();
	void OnKeyUp();
	void OnMouseButtonDown(SDL_Event&);
	void OnMouseButtonUp(SDL_Event&);

	int m_NumKeys;
	int m_NumKeysPrev;
	const Uint8* m_KeyboardState;
	std::vector<bool> m_MouseButtonStates;

};

#endif

