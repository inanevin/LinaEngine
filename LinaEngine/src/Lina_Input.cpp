#include "pch.h"
#include "Lina_Input.h"

Lina_Input::Lina_Input()
{
	//Add a console message about correct initialization
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Input initialized.", Lina_Console::MsgType::Success, "Input System");

	//Get state of the keyboard and also store the number of the keys in the keyboard at m_Numkeys
	m_KeyboardState = (Uint8*)SDL_GetKeyboardState(&m_NumKeys);

	//m_Keys will hold the current state of the keyboard.
	//Since at the start of the system there can't be pressed keys we initialize all of the previous keys false.
	for (int i = 0; i < m_NumKeys; i++)
	{
		m_Keys.push_back(m_KeyboardState[i]);
		m_PressedKeys.push_back(m_KeyboardState[i]);
	}
}

Lina_Input::~Lina_Input()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Input deinitialized.", Lina_Console::MsgType::Warning, "Input System");

	//Clearing the vectors does not de-allocate the all memory since while vectors dynamically grow they don't shrink,
	//So after clearing the vectors we can swap our vector with new empty. This will almost free all of the unused memory(Depends on the implementation of the vector).
	m_Keys.clear();
	std::vector<bool>().swap(m_Keys);

	m_PressedKeys.clear();
	std::vector<bool>().swap(m_PressedKeys);
}



void Lina_Input::Update()
{
	//This function will be called once in the loop an will sync the object's variables with the current state of the keyboard.

	//This function updates the event queue and internal device state.
	SDL_PumpEvents();
	
	m_PressedMouseButtons = m_MouseButtons;
	m_MouseButtons[0] = (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT));
	m_MouseButtons[1] = (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT));
	m_MouseButtons[2] = (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE));

	
	//After a frame we will assign the m_Keys to the m_PrevKeys as last frames keys and get the keyboards state for the current frame.
	for (int i = 0; i < m_NumKeys; i++)
	{
		m_PressedKeys[i] = m_Keys[i];
		m_Keys[i] = m_KeyboardState[i];
	}
}

bool Lina_Input::GetKey(int key)
{
	
	//Check if the pressed key is in the range of m_Numkeys
	if (key < 0 || key > m_NumKeys)
		return false;

	//If the key is pressed down return true.
	return(m_Keys[key]);
}

bool Lina_Input::GetKeyUp(int key)
{
	//Check if the pressed key is in the range of m_Numkeys
	if (key < 0 || key > m_NumKeys)
		return false;

	//If the key is pressed in the last frame and is not the same key in the current frame return true.
	return(m_PressedKeys[key] && !m_Keys[key]);
}

bool Lina_Input::GetKeyDown(int key)
{
	//Check if the pressed key is in the range of m_Numkeys
	if (key < 0 || key > m_NumKeys)
		return false;

	//If the key is pressed in the current frame and is not the same key in the last frame return true.
	return (m_Keys[key] && !m_PressedKeys[key]);
}

bool Lina_Input::GetMouseButton(int button)
{
	// Range check
	if (button < 0 || button > 2)
		return false;

	return m_MouseButtons[button];
}

bool Lina_Input::GetMouseButtonDown(int button)
{
	// Range check
	if (button < 0 || button > 2)
		return false;

	return m_MouseButtons[button] && !m_PressedMouseButtons[button];
}


bool Lina_Input::GetMouseButtonUp(int button)
{
	// Range check
	if (button < 0 || button > 2)
		return false;

	return !m_MouseButtons[button] && m_PressedMouseButtons[button];
}