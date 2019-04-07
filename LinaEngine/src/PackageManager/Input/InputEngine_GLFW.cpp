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
	}

	void InputEngine_GLFW::OnUpdate()
	{

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
	bool InputEngine_GLFW::GetMouseButton(int button)
	{
		int state = glfwGetMouseButton(glfwWindow, button);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	bool InputEngine_GLFW::GetMouseButtonDown(int button)
	{
		static int* oldState = new int[NUM_MOUSE_STATES];
		int newState = glfwGetMouseButton(glfwWindow, button);
		bool flag = (newState == GLFW_PRESS && oldState[button] == GLFW_RELEASE) ? true : false;
		oldState[button] = newState;
		return flag;
	}
	bool InputEngine_GLFW::GetMouseButtonUp(int button)
	{
		static int* oldState = new int[NUM_MOUSE_STATES];
		int newState = glfwGetMouseButton(glfwWindow, button);
		bool flag = (newState == GLFW_RELEASE && oldState[button] == GLFW_PRESS) ? true : false;
		oldState[button] = newState;
		return flag;
	}

	Vector2F InputEngine_GLFW::GetRawMouseAxis()
	{
		double posX, posY;
		glfwGetCursorPos(glfwWindow, &posX, &posY);
		static Vector2F oldMousePosition;
		Vector2F currentMousePosition = Vector2F(posX, posY);
		Vector2F diff = currentMousePosition - oldMousePosition;
		Vector2F raw = Vector2F::Zero();
		if (diff.x > 0.0f) raw.x = 1.0f;
		else if (diff.x < 0.0f) raw.x = -1.0f;
		if (diff.y > 0) raw.y = -1.0f;
		else if (diff.y < 0) raw.y = 1.0f;
		oldMousePosition = currentMousePosition;
		return raw;
	}

	Vector2F InputEngine_GLFW::GetMouseAxis()
	{
		// TODO: This returns the raw mouse axis for now, smooth the raw & map between -1 & 1 later.
		return GetRawMouseAxis();
	}

	Vector2F InputEngine_GLFW::GetMousePosition()
	{
		double xpos, ypos;
		glfwGetCursorPos(glfwWindow, &xpos, &ypos);
		return Vector2F(xpos, ypos);
	}

	void InputEngine_GLFW::SetCursor(bool visible) const
	{
		if (visible)
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}

	void InputEngine_GLFW::SetMousePosition(const Vector2F & v) const
	{
		glfwSetCursorPos(glfwWindow, v.x, v.y);
	}


	void InputEngine_GLFW::DispatchKeyAction(Input::Key key, int action)
	{
		if (action == GLFW_PRESS)
		{
			Action<Input::Key> keyPress = Action<Input::Key>(KeyPressed);
			keyPress.SetData(key);
			m_ActionDispatcher.DispatchAction(keyPress);
		}
		else if (action == GLFW_RELEASE)
		{
			Action<Input::Key> keyPress = Action<Input::Key>(KeyReleased);
			keyPress.SetData(key);
			m_ActionDispatcher.DispatchAction(keyPress);
		}
	}

	void InputEngine_GLFW::DispatchMouseAction(Input::Mouse button, int action)
	{
		if (action == GLFW_PRESS)
		{
			Action<Input::Mouse> mousePress = Action<Input::Mouse>(MouseButtonPressed);
			mousePress.SetData(button);
			m_ActionDispatcher.DispatchAction(mousePress);
		}
		else if (action == GLFW_RELEASE)
		{
			Action<Input::Mouse> mousePress = Action<Input::Mouse>(MouseButtonReleased);
			mousePress.SetData(button);
			m_ActionDispatcher.DispatchAction(mousePress);
		}
	}
}

#endif