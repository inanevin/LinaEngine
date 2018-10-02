#include "pch.h"
#include "Lina_Input.h"

#include <SDL2/SDL.h>

Lina_Input::Lina_Input()
{
	//Add a console message about correct initialization
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Input initialized.", Lina_Console::MsgType::Success);

	//Get state of the keyboard and also store the number of the keys in the keyboard at m_Numkeys
	Uint8* keyboard = (Uint8*)SDL_GetKeyboardState(&m_NumKeys);

	//Allocate memory for all of the keys and Previously used keys
	m_Keys = new bool[m_NumKeys];
	m_PrevKeys = new bool[m_NumKeys];

	//m_Keys will hold the current state of the keyboard.
	//Since at the start of the system there can't be pressed keys we initialize all of the previous keys false.
	for (int i = 0; i < m_NumKeys; i++)
	{
		m_Keys[i] = keyboard[i];
		m_PrevKeys[i] = false;
	}
}

Lina_Input::~Lina_Input()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Input deinitialized.", Lina_Console::MsgType::Warning);

	delete[] m_Keys;
	delete[] m_PrevKeys;
}

void Lina_Input::Update()
{
	//This function will be called once in the loop an will sync the object's variables with the current state of the keyboard.

	//Get state of the keyboard
	Uint8* keyboard = (Uint8*)SDL_GetKeyboardState(&m_NumKeys);

	//After a frame we will assign the m_Keys to the m_PrevKeys as last frames keys and get the keyboards state for the current frame.
	for (int i = 0; i < m_NumKeys; i++)
	{
		m_PrevKeys[i] = m_Keys[i];
		m_Keys[i] = keyboard[i];
	}
}

bool Lina_Input::GetKey(int key)
{
	//Check if the pressed key is in the range of m_Numkeys
	if (key < 0 || key > m_NumKeys)
		return false;

	//If the key is pressed in the current frame and is not the same key in the last frame return true.
	return (m_Keys[key] && !m_PrevKeys[key]);
}

bool Lina_Input::GetKeyUp(int key)
{
	//Check if the pressed key is in the range of m_Numkeys
	if (key < 0 || key > m_NumKeys)
		return false;

	//If the key is pressed in the last frame and is not the same key in the current frame return true.
	return(m_PrevKeys[key] && !m_Keys[key]);
}

bool Lina_Input::GetKeyDown(int key)
{
	//Check if the pressed key is in the range of m_Numkeys
	if (key < 0 || key > m_NumKeys)
		return false;

	//If the key is pressed down return true.
	return(m_Keys[key]);
}

