#pragma once

#ifndef Lina_Input_H
#define Lina_Input_H

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
	bool* m_Keys;
	bool* m_PrevKeys;
	int m_NumKeys;
};

#endif
