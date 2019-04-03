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


	InputEngine_GLFW::~InputEngine_GLFW()
	{
		delete previousKeys;
		delete currentKeys;
	}

	void InputEngine_GLFW::Initialize()
	{
		glfwWindow = static_cast<GLFWwindow*>(Application::Get().GetRenderingEngine().GetMainWindow().GetNativeWindow());
		LINA_CORE_ENSURE_ASSERT(glfwWindow != NULL, , "GLFW Window is null!");
		LINA_CORE_INFO("Input Engine GLFW Initialized");


		auto keyPressedFunc = [](GLFWwindow* w, int key, int scancode, int action, int mods)
		{
			static_cast<InputEngine_GLFW*>(glfwGetWindowUserPointer(w))->KeyCallback(w, key, scancode, action, mods);
		};

		auto mousePressedFunc = [](GLFWwindow* w, int button, int action, int mods)
		{
			static_cast<InputEngine_GLFW*>(glfwGetWindowUserPointer(w))->MouseCallback(w, button, action, mods);
		};


		glfwSetKeyCallback(glfwWindow, keyPressedFunc);
		glfwSetMouseButtonCallback(glfwWindow, mousePressedFunc);
	}

	void InputEngine_GLFW::OnUpdate()
	{
	
		//memcpy(previousKeys, currentKeys, sizeof(bool) * NUM_KEY_STATES);

	/*
		// Poll Events
		glfwPollEvents();

		// Close window if Escape is pressed.
		if (glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(glfwWindow, true);

		/* USE PREPROCESSORS TO CHECK DEBUG OR RELEASE LATER! */
		// Toggle wireframe mode using F1.
		/*if (glfwGetKey(glfwWindow, GLFW_KEY_F1) == GLFW_PRESS)
			WireframeModeToggle();
		if (glfwGetKey(glfwWindow, GLFW_KEY_F2) == GLFW_PRESS)
		{
			Action<int> keyPress = Action<int>(KeyPressed);
			keyPress.SetData(GLFW_KEY_F2);
			m_ActionDispatcher.DispatchAction(keyPress);
		}
		if (glfwGetKey(glfwWindow, GLFW_KEY_F3) == GLFW_PRESS)
		{
			Action<int> keyPress = Action<int>(KeyPressed);
			keyPress.SetData(GLFW_KEY_F3);
			m_ActionDispatcher.DispatchAction(keyPress);
		}
	*/
	}

	bool InputEngine_GLFW::GetKey(int keycode)
	{
		int state = glfwGetKey(glfwWindow, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool InputEngine_GLFW::GetKeyDown(int keyCode)
	{
		static int* oldState = new int[NUM_KEY_STATES];
		int newState = glfwGetKey(glfwWindow, keyCode);
		bool flag = (newState == GLFW_PRESS && oldState[keyCode] == GLFW_RELEASE) ? true : false;
		oldState[keyCode] = newState;
		return flag;
	}
	bool InputEngine_GLFW::GetKeyUp(int keyCode)
	{
		static int* oldState = new int[NUM_KEY_STATES];
		int newState = glfwGetKey(glfwWindow, keyCode);
		bool flag = (newState == GLFW_RELEASE && oldState[keyCode] == GLFW_PRESS) ? true : false;
		oldState[keyCode] = newState;
		return flag;
	}
	bool InputEngine_GLFW::GetMouse(int button)
	{
		int state = glfwGetMouseButton(glfwWindow, button);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	bool InputEngine_GLFW::GetMouseDown(int button)
	{
		static int* oldState = new int[NUM_MOUSE_STATES];
		int newState = glfwGetMouseButton(glfwWindow, button);
		bool flag = (newState == GLFW_PRESS && oldState[button] == GLFW_RELEASE) ? true : false;
		oldState[button] = newState;
		return flag;
	}
	bool InputEngine_GLFW::GetMouseUp(int button)
	{
		static int* oldState = new int[NUM_MOUSE_STATES];
		int newState = glfwGetMouseButton(glfwWindow, button);
		bool flag = (newState == GLFW_RELEASE && oldState[button] == GLFW_PRESS) ? true : false;
		oldState[button] = newState;
		return flag;
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
		double xpos, ypos;
		glfwGetCursorPos(glfwWindow, &xpos, &ypos);
		return Vector2F(xpos, ypos);
	}

	void InputEngine_GLFW::SetCursor(bool visible) const
	{

	}

	void InputEngine_GLFW::SetMousePosition(const Vector2F & v) const
	{

	}

	void InputEngine_GLFW::KeyCallback(GLFWwindow * w, int key, int scancode, int action, int mods)
	{

	}

	void InputEngine_GLFW::MouseCallback(GLFWwindow * w, int button, int action, int mods)
	{

	}
}

#endif