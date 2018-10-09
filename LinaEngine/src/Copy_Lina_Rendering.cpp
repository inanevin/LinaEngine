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
9/30/2018 4:59:52 PM

*/

#include "pch.h"
#include "Copy_Lina_Rendering.h"  

/*Lina_Rendering& Lina_Rendering::Instance()
{
static Lina_Rendering instance;
return instance;
}*/

Copy_Lina_Rendering::Copy_Lina_Rendering(const std::shared_ptr<Lina_InputHandler>& inp)
{
	// Add a console message.
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Rendering engine initialized.", Lina_Console::MsgType::Initialization, "Render Engine");

	inputEngine = inp;

	m_Scene = std::make_shared<Lina_Scene>();
}

// Destructor.
Copy_Lina_Rendering::~Copy_Lina_Rendering()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Rendering deinitialized.", Lina_Console::MsgType::Deinitialization, "Render Engine");
}

// Main method to render a particular image on the active window.
void Copy_Lina_Rendering::Render()
{
	// Send dbg msg.
	/* Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Render Loop is running", Lina_Console::MsgType::Update, "Render Engine", true); */

	// Check if active window points to an object.
	if (m_ActiveWindow == nullptr)
	{
		//cons.AddConsoleMsg("No active window to render onto!", Lina_Console::MsgType::Error, "Render Engine");
		return;
	}

	// Close the active window upon escape key press.
	//	if (inputEngine->GetKeyDown(SDL_SCANCODE_ESCAPE))
	//m_ActiveWindow->CloseWindow();

	//Render loop should do three things in order. Clear the buffers, draw the meshes and swapping the buffers.
	// Call render method on active window.
	m_ActiveWindow->RenderBlankColor(); //the Update call from this function should be removed since it causes 2 swapbuffer calls in a single frame
										//and that causes flickering.
	m_Scene->Draw();

	m_ActiveWindow->Update();

}

void Copy_Lina_Rendering::CleanUp()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Cleaning up rendering engine...", Lina_Console::MsgType::Deinitialization, "Render Engine");

	// If we have an active window decrement the shared pointer of it.
	if (m_ActiveWindow != nullptr)
		m_ActiveWindow.reset();

	m_Scene->CleanUp();
}

void Copy_Lina_Rendering::CreateDisplayWindow(int width, int height, const std::string& title)
{
	// Initialize display.
	m_ActiveWindow = std::make_shared<Lina_Window>(width, height, title);
}


