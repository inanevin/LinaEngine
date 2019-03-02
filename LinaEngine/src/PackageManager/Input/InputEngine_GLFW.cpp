/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: InputEngine_GLFW
Timestamp: 2/25/2019 9:43:54 AM

*/

#include "LinaPch.hpp"

#ifdef LLF_INPUTANDWINDOW_GLFW


#include "InputEngine_GLFW.hpp"  

#include "Lina/Application.hpp"
#include "Lina/Rendering/RenderingEngine.hpp"
#include "Lina/Rendering/Window.hpp"


namespace LinaEngine
{
	InputEngine_GLFW::InputEngine_GLFW()
	{

	}
	InputEngine_GLFW::~InputEngine_GLFW()
	{

	}

	void InputEngine_GLFW::Initialize()
	{

		// Check whether the window context is initialized or not, return if not.
		Window_GLFWGL* glfwWindowObj = dynamic_cast<Window_GLFWGL*>(&app->GetRenderingEngine().GetMainWindow());
		LINA_CORE_ENSURE_ASSERT(glfwWindowObj != NULL, , "GLFW Window Obj is null!");
		glfwWindow = glfwWindowObj->GetGLFWWindow();
		LINA_CORE_ENSURE_ASSERT(glfwWindow != NULL, , "GLFW Window is null!");
		LINA_CORE_ENSURE_ASSERT(glfwWindowObj->IsInitialized() != false, , "Window context is not initialized, quitting input engine initialization!");


		isInitialized = true;
	}

	void InputEngine_GLFW::OnUpdate()
	{
		if (!isInitialized) return;
	
		// Poll Events
		glfwPollEvents();

		// Close window if Escape is pressed.
		if (glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(glfwWindow, true);

		/* USE PREPROCESSORS TO CHECK DEBUG OR RELEASE LATER! */
		// Toggle wireframe mode using F1.
		if (glfwGetKey(glfwWindow, GLFW_KEY_F1) == GLFW_PRESS)
			WireframeModeToggle();
		if (glfwGetKey(glfwWindow, GLFW_KEY_F2) == GLFW_PRESS)
		{
			Action<int> keyPress = Action<int>(KeyPressed);
			keyPress.SetData(GLFW_KEY_F2);
			m_ActionDispatcher.DispatchAction(keyPress);
		}

	
	}

	bool InputEngine_GLFW::GetKey(int keyCode)
	{
		if (!isInitialized) return false;

	

		if (glfwGetKey(glfwWindow, keyCode) == GLFW_PRESS)
			return true;

		return false;
	}

	bool InputEngine_GLFW::GetKeyDown(int keyCode)
	{
		
		return false;
	}
	bool InputEngine_GLFW::GetKeyUp(int keyCode)
	{
		return false;
	}
	bool InputEngine_GLFW::GetMouse(int index)
	{
		return false;
	}
	bool InputEngine_GLFW::GetMouseDown(int index)
	{
		return false;
	}
	bool InputEngine_GLFW::GetMouseUp(int index)
	{
		return false;
	}
	Vector2F InputEngine_GLFW::GetRawMouseAxis()
	{
		return Vector2F();
	}
	Vector2F InputEngine_GLFW::GetMouseAxis()
	{
		return Vector2F();
	}
	Vector2F InputEngine_GLFW::GetMousePosition()
	{
		return Vector2F();
	}
	void InputEngine_GLFW::SetCursor(bool visible) const
	{
	}
	void InputEngine_GLFW::SetMousePosition(const Vector2F & v) const
	{
	}
}

#endif