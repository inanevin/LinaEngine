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
#include "Utility/Lina_Globals.h"
#include "Rendering/Lina_RenderingEngine.h"  


void Lina_RenderingEngine::CreateDisplayWindow(int width, int height, const std::string& title)
{
	// Initialize display.
	m_ActiveWindow = std::make_shared<Lina_Window>(width, height, title);
}

// Init rendering engine.
void Lina_RenderingEngine::Initialize(int width, int height, std::string title) 
{
	// Add a console message.
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Rendering engine initialized.", Lina_Console::MsgType::Initialization, "Render Engine");

	// Initialize event handler.
	eventHandler.Initialize();

	// Register window events.
	eventHandler.SubscribeToAction(ActionType::SDLQuit, [this]() { m_ActiveWindow->CloseWindow(); });

	// Create a window.
	CreateDisplayWindow(width, height, title);

	// Clear colors.
	ClearColors(0.0, 0.0, 0.0, 1.0);

	// Enable Textures
	SetTextures(true);

	// Every face in clock-wise order is front.
	glFrontFace(GL_CW);

	// Get rid of the back face.
	glCullFace(GL_BACK);

	// Enable culling faces for now to prevent extra faces.
	glEnable(GL_CULL_FACE);

	// Enable depth test -> enable Z draw-order.
	glEnable(GL_DEPTH_TEST);

	// depth clamp
	glEnable(GL_DEPTH_CLAMP);

	// Get free gamma correction.
	//glEnable(GL_FRAMEBUFFER_SRGB);

	// Init shaders
	InitializeShaders();
}

void Lina_RenderingEngine::InitializeShaders()
{
	phongShader.Init();
	basicShader.Init();
}

// Destructor.
Lina_RenderingEngine::~Lina_RenderingEngine()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Rendering deinitialized.", Lina_Console::MsgType::Deinitialization, "Render Engine");
}


void Lina_RenderingEngine::ClearScreen()
{
	// Clear color & depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Main method to render.
void Lina_RenderingEngine::Render()
{
	// Check if active window points to an object.
	if (m_ActiveWindow == nullptr)
	{
		return;
	}

	// Call render method on active window.
	m_ActiveWindow->Update();
}

void Lina_RenderingEngine::CleanUp()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Cleaning up rendering engine...", Lina_Console::MsgType::Deinitialization, "Render Engine");

	// If we have an active window decrement the shared pointer of it.
	if (m_ActiveWindow != nullptr)
		m_ActiveWindow.reset();

}

void Lina_RenderingEngine::SetTextures(bool enabled)
{
	if (enabled)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);
}

void Lina_RenderingEngine::ClearColors(float r, float g, float b, float a)
{
	// Clear color.
	glClearColor(r,g,b,a);
}


Lina_BasicShader* Lina_RenderingEngine::GetBasicShader()
{
	return &basicShader;
}

Lina_PhongShader* Lina_RenderingEngine::GetPhongShader()
{
	return &phongShader;
}



