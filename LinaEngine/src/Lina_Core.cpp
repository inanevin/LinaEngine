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


#include "pch.h"
#include <iostream>
#include <Lina_Core.h>
#include <Lina_Time.h>
#include <Lina_Actions.h>
#include <Lina_Vector3F.h>
#include <Lina_Matrix4F.h>
#include <Lina_Quaternion.h>

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

	// Wake the systems.
	Wake();

	// Start the game.
	Start();
}

// Destructor.
Lina_Core::~Lina_Core()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Core deinitialized.", Lina_Console::MsgType::Deinitialization, "Core");
}

// Initialization before start.
void Lina_Core::Wake()
{
	// Initialize Message Bus.
	Lina_CoreMessageBus::Instance().Initialize(&sdlHandler, &inputEngine, &renderingEngine);

	// Initialize SDL handler.
	sdlHandler.Initialize();

	// Initialize input engine.
	inputEngine.Initialize();

	// Initialize rendering engine.
	renderingEngine.Initialize();

	// Initialize game core.
	gameCore.Initialize();

	// Initialize event handler.
	objectHandler.Initialize();
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

	// For now the only condition is to have an active window to keep the rendering.
	while (isRunning)
	{
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
			if (renderingEngine.m_ActiveWindow == nullptr || renderingEngine.m_ActiveWindow->IsClosed())
			{
				Stop();
				break;
			}

			// Handle the input events.
			SDL_Event event;
			while (SDL_PollEvent(&event))
				inputEngine.HandleEvents(event);
			
			// Update the input engine.
			inputEngine.Update();

			// Set delta. (Change later, no effect for now)
			Lina_Time::SetDelta(frameTime);
			
			// TODO: Update game loop
			gameCore.ProcessInput();
			gameCore.Update();
			
			// print the frame counter every second.
			if (frameCounter >= SECOND)
			{
				// Debug frames.
				Lina_Console cons = Lina_Console();
				cons.AddConsoleMsg("Main Game Loop Running (" + std::to_string(frames) + " FPS)" + std::to_string(frames), Lina_Console::MsgType::Update, "Core Engine");
				// reset frame counter & frames to calculate on the next iteration.
				frames = 0;
				frameCounter = 0;
			}
			float x = objectHandler.GetRawMouseX();
			//	if(x != 0)
			//	std::cout << x << std::endl;
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
	renderingEngine.ClearScreen();
	gameCore.Render();
	renderingEngine.Render();
}

void Lina_Core::CleanUp()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Game engine loop cleaning up...", Lina_Console::MsgType::Deinitialization, "Core Engine");

	// Clean up render engine.
	renderingEngine.CleanUp();
}