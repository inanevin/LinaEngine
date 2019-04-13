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

#include "Lina/Core/Application.hpp"
#include "Lina/Rendering/RenderingEngine.hpp"
#include "Lina/Rendering/Window.hpp"


namespace LinaEngine
{

	static const float axisSensitivity = 0.1f;

	InputEngine_GLFW::InputEngine_GLFW()
	{
		LINA_CORE_TRACE("[Constructor] -> Input Engine GLFW ({0})", typeid(*this).name());
	}

	InputEngine_GLFW::~InputEngine_GLFW()
	{
		LINA_CORE_TRACE("[Destructor] -> Input Engine GLFW ({0})", typeid(*this).name());

		//delete previousKeys;
		//delete currentKeys;
	}

	void InputEngine_GLFW::Initialize()
	{

		InputEngine::Initialize();

		LINA_CORE_TRACE("[Initialization] -> Input Engine GLFW ({0})", typeid(*this).name());

		glfwWindow = static_cast<GLFWwindow*>(Application::Get().GetRenderingEngine().GetMainWindow().GetNativeWindow());
		LINA_CORE_ENSURE_ASSERT(glfwWindow != NULL, , "GLFW Window is null!");
		
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
		Vector2F raw = Vector2F::ZERO();
		if (diff.GetX() > 0.0f) raw.SetX(1.0f);
		else if (diff.GetX() < 0.0f) raw.SetX(-1.0f);
		if (diff.GetY() > 0) raw.SetY(-1.0f);
		else if (diff.GetY() < 0) raw.SetY(1.0f);
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
		glfwSetCursorPos(glfwWindow, v.GetX(), v.GetY());
	}


	void InputEngine_GLFW::DispatchKeyAction(Input::Key key, int action)
	{
		if (action == GLFW_PRESS)
		{
			DispatchAction<Input::Key>(ActionType::KeyPressed, key);
		}
		else if (action == GLFW_RELEASE)
		{
			DispatchAction<Input::Key>(ActionType::KeyReleased, key);
		}
	}

	void InputEngine_GLFW::DispatchMouseAction(Input::Mouse button, int action)
	{
		if (action == GLFW_PRESS)
		{
			DispatchAction<Input::Mouse>(ActionType::MouseButtonPressed, button);
		}
		else if (action == GLFW_RELEASE)
		{
			DispatchAction<Input::Mouse>(ActionType::MouseButtonReleased, button);
		}
	}
}

#endif