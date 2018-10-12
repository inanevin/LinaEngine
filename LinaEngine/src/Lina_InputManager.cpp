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
#include "Lina_InputManager.h"

Lina_InputManager::Lina_InputManager() : m_KeyboardState(0)
{
	//Add a console message about correct initialization
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Input initialized.", Lina_Console::MsgType::Success, "Input System");

	//Get state of the keyboard and also store the number of the keys in the keyboard at m_Numkeys
	//m_KeyboardState = (Uint8*)SDL_GetKeyboardState(&m_NumKeys);
	
	//m_Keys = new Uint8[m_NumKeys];
	//m_PressedKeys = new Uint8[m_NumKeys];
	//m_Keys will hold the current state of the keyboard.
	//Since at the start of the system there can't be pressed keys we initialize all of the previous keys false.
	for (int i = 0; i < 3; i++)
	{
		m_MouseButtonStates.push_back(false);
	}
}

Lina_InputManager::~Lina_InputManager()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Input deinitialized.", Lina_Console::MsgType::Warning, "Input System");

	delete m_KeyboardState;
	m_MouseButtonStates.clear();
	std::vector<bool>().swap(m_MouseButtonStates);
	//Clearing the vectors does not de-allocate the all memory since while vectors dynamically grow they don't shrink,
	//So after clearing the vectors we can swap our vector with new empty. This will almost free all of the unused memory(Depends on the implementation of the vector).
	//m_Keys.clear();
	//std::vector<bool>().swap(m_Keys);

	//m_PressedKeys.clear();
	//std::vector<bool>().swap(m_PressedKeys);
}

bool Lina_InputManager::GetKeyDown(SDL_Scancode key) const
{
	if (m_KeyboardState != 0 && m_KeyboardState[key] == 1) return true;

	return false;
}

bool Lina_InputManager::GetMouseButtonDown(int button) const
{
	return m_MouseButtonStates[button];
}


void Lina_InputManager::Update()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {

		case SDL_MOUSEBUTTONDOWN:
			OnMouseButtonDown(event);
			break;

		case SDL_MOUSEBUTTONUP:
			OnMouseButtonUp(event);
			break;

		case SDL_KEYDOWN:
			OnKeyDown();
			break;

		case SDL_KEYUP:
			OnKeyUp();
			break;

		default:
			break;
		}
	}

	//This function will be called once in the loop an will sync the object's variables with the current state of the keyboard.

	//This function updates the event queue and internal device state.
	/*SDL_PumpEvents();
	
	m_PressedMouseButtons = m_MouseButtons;
	m_MouseButtons[0] = (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT));
	m_MouseButtons[1] = (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT));
	m_MouseButtons[2] = (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE));*/

	//m_Keys = new Uint8[m_NumKeysPrev];
	//m_PressedKeys = new Uint8[m_NumKeys];
	//memcpy(m_PressedKeys, m_Keys, m_NumKeysPrev);
	//memcpy(m_Keys, m_KeyboardState, m_NumKeys);
	//m_NumKeysPrev = m_NumKeys;

//	std::copy(std::begin(m_Keys), std::end(m_PressedKeys), std::begin(m_Keys));
	//After a frame we will assign the m_Keys to the m_PrevKeys as last frames keys and get the keyboards state for the current frame.
	/*for (int i = 0; i < m_NumKeys; i++)
	{
	//	m_PressedKeys[i] = m_Keys[i];
		//m_Keys[i] = m_KeyboardState[i];
	}*/
}


void Lina_InputManager::OnKeyDown()
{
	m_KeyboardState = SDL_GetKeyboardState(0);
}

void Lina_InputManager::OnKeyUp()
{
	m_KeyboardState = SDL_GetKeyboardState(0);
}

void Lina_InputManager::OnMouseButtonDown(SDL_Event &event)
{
	if (event.button.button == SDL_BUTTON_LEFT) {
		m_MouseButtonStates[LEFT] = true;
	}

	if (event.button.button == SDL_BUTTON_MIDDLE) {
		m_MouseButtonStates[MIDDLE] = true;
	}

	if (event.button.button == SDL_BUTTON_RIGHT) {
		m_MouseButtonStates[RIGHT] = true;
	}
}

void Lina_InputManager::OnMouseButtonUp(SDL_Event &event)
{
	if (event.button.button == SDL_BUTTON_LEFT) {
		m_MouseButtonStates[LEFT] = false;
	}

	if (event.button.button == SDL_BUTTON_MIDDLE) {
		m_MouseButtonStates[MIDDLE] = false;
	}

	if (event.button.button == SDL_BUTTON_RIGHT) {
		m_MouseButtonStates[RIGHT] = false;
	}
}


/*bool Lina_InputManager::GetKey(int key)
{
	
	//Check if the pressed key is in the range of m_Numkeys
	if (key < 0 || key > m_NumKeys)
		return false;

	//If the key is pressed down return true.
	return(m_Keys[key]);
}

bool Lina_InputManager::GetKeyUp(int key)
{
	//Check if the pressed key is in the range of m_Numkeys
	if (key < 0 || key > m_NumKeys)
		return false;

	//If the key is pressed in the last frame and is not the same key in the current frame return true.
	return(m_PressedKeys[key] && !m_Keys[key]);
}

bool Lina_InputManager::GetKeyDown(int key)
{
	//Check if the pressed key is in the range of m_Numkeys
	if (key < 0 || key > m_NumKeys)
		return false;

	//If the key is pressed in the current frame and is not the same key in the last frame return true.
	return (m_Keys[key] && !m_PressedKeys[key]);
}

bool Lina_InputManager::GetMouseButton(int button)
{
	// Range check
	if (button < 0 || button > 2)
		return false;

	return m_MouseButtons[button];
}

bool Lina_InputManager::GetMouseButtonDown(int button)
{
	// Range check
	if (button < 0 || button > 2)
		return false;

	return m_MouseButtons[button] && !m_PressedMouseButtons[button];
}


bool Lina_InputManager::GetMouseButtonUp(int button)
{
	// Range check
	if (button < 0 || button > 2)
		return false;

	return !m_MouseButtons[button] && m_PressedMouseButtons[button];
}*/