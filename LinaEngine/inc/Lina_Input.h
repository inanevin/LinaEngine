#pragma once

#ifndef Lina_Input_H
#define Lina_Input_H

#include <SDL2/SDL.h>

#include <vector>

class Lina_Input
{
public:
	Lina_Input();
	~Lina_Input();

	void Update();
	bool GetKey(int key);
	bool GetKeyUp(int key);
	bool GetKeyDown(int key);

private:
	std::vector<bool> m_Keys;
	std::vector<bool> m_PressedKeys;
	int m_NumKeys;

	Uint8* m_KeyboardState;
};

#endif
