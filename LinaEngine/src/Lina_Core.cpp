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

*/


#include "pch.h"
#include <iostream>
#include <Lina_Core.h>
#include <Lina_Time.h>

static const double FRAME_CAP = 5000.0;	// max frame limit we can draw. (ex.5000 frames in a sec)
static const long SECOND = 1000000000;	// time in nanosecs

// Constructor, initialize components.
Lina_Core::Lina_Core()
{
	// Add a console message.
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Core initialized.", Lina_Console::MsgType::Initialization, "Core");

	// Set running.
	isRunning = false;

	// Initialize SDL handler.
	sdlHandler = std::make_shared<Lina_SDLHandler>();

	// Initialize input engine.
	inputEngine = std::make_shared<Lina_InputHandler>();

	// Initialize rendering engine.
	renderingEngine = std::make_shared<Lina_Rendering>(inputEngine);

	// Initialize game core.
	gameCore = std::make_shared<Lina_GameCore>(inputEngine);

	// Create a window.
	renderingEngine->CreateDisplayWindow(1024, 768, "Lina Engine 3D");

	// Start the game.
	Start();
}



// Destructor.
Lina_Core::~Lina_Core()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Core deinitialized.", Lina_Console::MsgType::Deinitialization, "Core");
}

// Initialization method for the game core.
void Lina_Core::Start()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Game engine loop starting...", Lina_Console::MsgType::Initialization, "Core Engine");

	// Can not start if we are already running.
	if (isRunning) return;

	// Start the main game loop.
	Run();
}

void Lina_Core::Stop()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Game engine loop stopping...", Lina_Console::MsgType::Deinitialization, "Core Engine");

	// Can not stop if we are not running.
	if (!isRunning) return;

	// Stop running the engine.
	isRunning = false;
}



void TestMethod1()
{
	std::cout << "sa";

}
void TestMethod22()
{
	Lina_Console cons = Lina_Console();

	cons.AddConsoleMsg("SA", Lina_Console::MsgType::Update, "2Core Engine");

}
void TestMethod33()
{
	std::cout << "TestMethod33";

}


void TestMethod44()
{
	std::cout << "TestMethod44";

}
void TestMethod2()
{
	//h->UnBindMethod(&TestMethod1);
	std::cout << "TestMethod2";
}



void Lina_Core::Run()
{
	isRunning = true;

	// Amount of time one frame takes.
	const double frameTime = 1.0 / FRAME_CAP;

	// Time that prev frame started running.
	long lastTime = Lina_Time::GetCurrentTimeInNano();

	// Cumilator -> Keep track of how much times we still need to update the game.
	double unprocessedTime = 0;

	// Frame counter.
	int frames = 0;
	long frameCounter = 0;

	//Lina_InputBinding b5(Lina_InputBinding::MouseEventType::OnMouseButton, 0, &TestMethod2);
	//Lina_InputBinding b3(Lina_InputBinding::KeyboardEventType::OnKeyDown, SDL_SCANCODE_F, &TestMethod2);
	
	Lina_InputBinding b(InputEventType::OnMouseButtonUp, MouseButton::MOUSE_LEFT, &TestMethod1);
	//Lina_InputBinding b2(InputEventType::OnMouseButtonDown, 1, &TestMethod22);
	Lina_InputBinding b3(InputEventType::OnMouseButtonDown, MouseButton::MOUSE_LEFT, &TestMethod33);
	Lina_InputBinding b4(InputEventType::OnMouseButton, MouseButton::MOUSE_LEFT, &TestMethod44);
	inputEngine->Bind(b);
	//inputEngine->Bind(b2);
inputEngine->Bind(b3);
	inputEngine->Bind(b4);

	//auto f2 = [this, &b, &b3] { TestMethod3(b, b3); };
	//Lina_InputBinding b2(Lina_InputBinding::KeyboardEventType::OnKey, SDL_SCANCODE_B, f2);

	//inputEngine->Bind(b);
	//inputEngine->Bind(b2);
	//inputEngine->Bind(b3);
	
	//auto&& f = std::bind(TestMethod2, 5, 2, inputEngine);
	//inputEngine->BindMethod(Lina_InputHandler::EventType::OnKeyPress, SDL_SCANCODE_B, f, "a1");


	//inputEngine->BindMethod(Lina_InputHandler::EventType::OnKeyPress, SDL_SCANCODE_C, f2, "a3");


	// For now the only condition is to have an active window to keep the rendering.
	while (isRunning)
	{
		// Debug running.
		/*Lina_Console cons = Lina_Console();
		cons.AddConsoleMsg("Game engine loop running...", Lina_Console::MsgType::Update, "Core Engine", true); */

		// Whether to render the frame or not.
		bool renderFrame = false;
	

		// Time that this frame started running.
		long startTime = Lina_Time::GetCurrentTimeInNano();

		// amount of the it took for the frame.
		long passedTime = startTime - lastTime;
		//std::cout << passedTime << std::endl;

		// this frame is now the previous frame.
		lastTime = startTime;

		// How much time has passed in doubles. (running total of time passed)
		unprocessedTime += passedTime / static_cast<double>(SECOND);

		// Increment frame counter.
		frameCounter += passedTime;

		// While total time is greater than time one frame is supposed to take. (update time)
		while (unprocessedTime > frameTime)
		{

			renderFrame = true;

			// decrease time to process.
			unprocessedTime -= frameTime;

			// If we don't have an active window or is closed stop.
			if (renderingEngine->m_ActiveWindow == nullptr || renderingEngine->m_ActiveWindow->IsClosed())
			{
				Stop();
				break;
			}

			inputEngine->Update();

			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				inputEngine->HandleEvents(event);

			}

			// Set delta. (Change later, no effect for now)
			Lina_Time::SetDelta(frameTime);
		
			// Update SDL Handler.
			sdlHandler->Process();

			//std::cout << menu;
		
			// TODO: Update game loop
			gameCore->ProcessInput();
			gameCore->Update();
			// print the frame counter every second.
			if (frameCounter >= SECOND)
			{
				// Debug frames.
				Lina_Console cons = Lina_Console();
				//cons.AddConsoleMsgSameLine("Main Game Loop Running (" + std::to_string(frames) + " FPS)" + std::to_string(frames), Lina_Console::MsgType::Update, "Core Engine");
				// reset frame counter & frames to calculate on the next iteration.
				frames = 0;
				frameCounter = 0;
			}

		}

		// render the frame.
		if (renderFrame)
		{
			Render();
			frames++;	// Increment the amount of frames rendered.
		}
		else
		{
			try {
				// Sleep for a single millisecond instead waiting for rendering and running calculations.
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			catch (const std::exception& e) {
				// Debug exception.
				Lina_Console cons = Lina_Console();
				cons.AddConsoleMsg(e.what(), Lina_Console::MsgType::Error, "Core Engine");
			}
		}


	}

	// Clean up thrash when finishing running.
	CleanUp();
}

// Rendering loop.
void Lina_Core::Render()
{

	gameCore->Render();
	renderingEngine->Render();
}

void Lina_Core::CleanUp()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Game engine loop cleaning up...", Lina_Console::MsgType::Deinitialization, "Core Engine");

	// Clean up render engine.
	renderingEngine->CleanUp();
}