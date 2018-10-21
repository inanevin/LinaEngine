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
10/12/2018 1:50:18 AM

*/

#include "pch.h"
#include "Core/Lina_CoreMessageBus.h"  
#include "Rendering/Lina_RenderingEngine.h"
#include "Input/Lina_InputEngine.h"

void Lina_CoreMessageBus::Initialize()
{
	Lina_Console c;
	c.AddConsoleMsg("Message Bus Initialized", Lina_Console::MsgType::Initialization, "Message Bus");
}

Lina_InputEngine* Lina_CoreMessageBus::GetInputEngine()
{
	return inputEngine;
}

Lina_RenderingEngine* Lina_CoreMessageBus::GetRenderingEngine()
{
	return renderingEngine;
}

Lina_Core* Lina_CoreMessageBus::GetCoreEngine()
{
	return coreEngine;
}

void Lina_CoreMessageBus::SetInputEngine(Lina_InputEngine* inp)
{
	inputEngine = inp;
}

void Lina_CoreMessageBus::SetRenderingEngine(Lina_RenderingEngine* rnd)
{
	renderingEngine = rnd;
}

void Lina_CoreMessageBus::SetCore(Lina_Core* c)
{
	coreEngine = c;
}
