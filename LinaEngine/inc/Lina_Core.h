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


*/

#pragma once

#ifndef LINA_CORE_H
#define LINA_CORE_H

#include "Lina_RenderingEngine.h"
#include "Lina_GameCore.h"
#include "Lina_InputEngine.h"
#include "Lina_ObjectHandler.h"

extern const double FRAME_CAP;
extern const long SECOND;

class Lina_Core {

public:

    Lina_Core();
	~Lina_Core();

private:

	void Wake();
	void Start();
	void Stop();
	void Run();
	void Render();
	void CleanUp();
	bool isRunning;

	Lina_InputEngine inputEngine;
	Lina_RenderingEngine renderingEngine;
	Lina_GameCore gameCore;
	Lina_SDLHandler sdlHandler;

	// Object Specific
	Lina_ObjectHandler eventHandler;

	Lina_Core(const Lina_Core& r) = delete;


};

#endif
