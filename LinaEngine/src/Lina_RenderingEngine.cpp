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
9/30/2018 4:59:52 PM

*/

#include "pch.h"
#include "Lina_RenderingEngine.h"  

// Init rendering engine.
void Lina_RenderingEngine::Initialize() 
{
	// Add a console message.
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Rendering engine initialized.", Lina_Console::MsgType::Initialization, "Render Engine");

	// Initialize event handler.
	eventHandler.Initialize();

	// Register window events.
	eventHandler.SubscribeToAction(ActionType::SDLQuit, [this]() { m_ActiveWindow->CloseWindow(); });

	// Create a window.
	CreateDisplayWindow(1024, 768, "Lina Engine 3D");

	// Init my scene
	m_Scene.InitScene();
}


// Destructor.
Lina_RenderingEngine::~Lina_RenderingEngine()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Rendering deinitialized.", Lina_Console::MsgType::Deinitialization, "Render Engine");
}

// Main method to render a particular image on the active window.
void Lina_RenderingEngine::Render()
{
	// Check if active window points to an object.
	if (m_ActiveWindow == nullptr)
	{
		return;
	}

	// Call render method on active window.
	//m_ActiveWindow->RenderBlankColor();

	m_Scene.Draw();

	m_ActiveWindow->Update();
}

void Lina_RenderingEngine::CleanUp()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Cleaning up rendering engine...", Lina_Console::MsgType::Deinitialization, "Render Engine");

	// If we have an active window decrement the shared pointer of it.
	if (m_ActiveWindow != nullptr)
		m_ActiveWindow.reset();

	m_Scene.CleanUp();
}

void Lina_RenderingEngine::CreateDisplayWindow(int width, int height, const std::string& title)
{
	// Initialize display.
	m_ActiveWindow = std::make_shared<Lina_Window>(width,height,title);
}


