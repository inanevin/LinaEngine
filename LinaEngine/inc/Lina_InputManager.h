
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

