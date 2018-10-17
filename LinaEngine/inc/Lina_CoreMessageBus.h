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
10/9/2018 4:44:45 AM

*/

#pragma once

#ifndef Lina_CoreMessageBus_H
#define Lina_CoreMessageBus_H

#include "Input/Lina_InputEngine.h"
class Lina_Core;
class Lina_RenderingEngine;
class Lina_SDLHandler;

class Lina_CoreMessageBus
{
public:

	void Initialize(Lina_SDLHandler*, Lina_InputEngine*, Lina_RenderingEngine*);

	static Lina_CoreMessageBus& Instance()
	{
		static Lina_CoreMessageBus instance; 				  
		return instance;
	}

	Lina_InputEngine* GetInputEngine() { return inputEngine; }
	void SetInputEngine(Lina_InputEngine* inp) { inputEngine = inp; }

private:

	Lina_CoreMessageBus() {}

	/* INSTANCES */
	Lina_InputEngine* inputEngine;
	Lina_SDLHandler* sdlHandler;
	Lina_Core* coreEngine;
	Lina_RenderingEngine* renderingEngine;

public:

	Lina_CoreMessageBus(Lina_CoreMessageBus const&) = delete;
	void operator=(Lina_CoreMessageBus const&) = delete;

};


#endif