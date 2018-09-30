#include "pch.h"
#include "Lina_Input.h"

#include <SDL2/SDL.h>

Lina_Input::Lina_Input()
{
	//Add a console message about correct initialization
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Input initialized.", Lina_Console::MsgType::Success);
}

Lina_Input::~Lina_Input()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Input deinitialized.", Lina_Console::MsgType::Warning);
}

void Lina_Input::ProcessInput()
{
	//Process the keyboard and mouse inputs in here
	
	//TODO: Find a suitable Lina_Engine Component for using Lina_Input(Lina_Window or Lina_Renderer)
	
	//Get State of Keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	
	if (state[SDL_SCANCODE_A])
	{
		Lina_Console cons = Lina_Console();
		cons.AddConsoleMsg("Pressed: A", Lina_Console::MsgType::Debug);
	}
}
