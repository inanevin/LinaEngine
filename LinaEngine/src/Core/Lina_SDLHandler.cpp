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
10/2/2018 11:40:43 AM

*/

#include "pch.h"
#include "Core/Lina_SDLHandler.h"  

void Lina_SDLHandler::Initialize()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("SDL Handler initialized.", Lina_Console::Initialization, "SDL Handler");

	// Initialize SDL.
	SDL_Init(SDL_INIT_EVERYTHING);
	cons.AddConsoleMsg("SDL initialized with SDL_INIT_EVERYTHING.", Lina_Console::Initialization, "SDL Handler");

	// Set Mouse Relative Motion
	//SDL_SetRelativeMouseMode(SDL_TRUE);
}

Lina_SDLHandler::~Lina_SDLHandler()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("SDL Quitted.", Lina_Console::Initialization, "SDL Handler");
	SDL_Quit();
}