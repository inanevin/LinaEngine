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

/*
Class: EditorApplication

This class can also be derived from Lina::Application to create a standalone editor build.
Probably will be deprecated in the future as the editor is now a linked-library from Sandbox.

Timestamp: 10/15/2020 10:44:39 PM
*/

#pragma once

#ifndef EditorApplication_HPP
#define EditorApplication_HPP

#include "Log/Log.hpp"
#include "ECS/Systems/EditorCameraSystem.hpp"
#include "Core/GUILayer.hpp"
#include "EventSystem/Events.hpp"

namespace Lina
{
	namespace World
	{
		class Level;
	}

}

namespace Lina::Editor
{
	class EditorApplication
	{
	public:

		EditorApplication() {};
		~EditorApplication() {};

		void Initialize();
		void Refresh();
		void LevelInitialized(Event::ELevelInitialized ev);

	private:

		void PlayModeChanged(Event::EPlayModeChanged playmode);

	private:

		Lina::ECS::EditorCameraSystem editorCameraSystem;
		GUILayer m_guiLayer;
	};
}

#endif
