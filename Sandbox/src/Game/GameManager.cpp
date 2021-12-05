/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Game/GameManager.hpp"
#include "Core/Application.hpp"
#include "Levels/Example1Level.hpp"

namespace LinaEngine
{
	GameManager* GameManager::s_instance = nullptr;
	Example1Level* m_exampleLevel = nullptr;

	void GameManager::Attach()
	{
		s_instance = this;
		LINA_CLIENT_TRACE("Game Manager Attached!");
		m_exampleLevel = new Example1Level();
		LinaEngine::Application::GetApp().InstallLevel(*m_exampleLevel, true, "resources/sandbox/Levels/", "Example1Level");
	}

	void GameManager::Tick(float delta)
	{

	}



}