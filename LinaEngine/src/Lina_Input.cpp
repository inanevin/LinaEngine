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
	
	//TODO: change the ProcessInput class to something compatible with the SDL_Event system.
	
	//Get State of Keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	
	if (state[SDL_SCANCODE_A])
	{
		Lina_Console cons = Lina_Console();
		cons.AddConsoleMsg("Pressed: A", Lina_Console::MsgType::Debug);
	}

	//Get State of Mouse
	if (SDL_GetMouseState(NULL, NULL) && SDL_BUTTON(1))
	{
		Lina_Console cons = Lina_Console();
		cons.AddConsoleMsg("Left Mouse Click", Lina_Console::MsgType::Debug);
	}

	//While states of keyboard and mouse can be seen from the Lina_Window, right now because of not using the SDL_Event system
	//SDL does not take SDL_KEYDOWN and SDL_KEYUP into consideration thus it inputs as much as it can in a frame.

}
