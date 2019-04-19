/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: GLInputEngine
Timestamp: 4/14/2019 5:15:15 PM

*/

#include "LinaPch.hpp"
#include "PackageManager/OpenGL/GLInputEngine.hpp"  
#include "GLFW/glfw3.h"


namespace LinaEngine::Input
{

	static const float axisSensitivity = 0.1f;
	GLFWwindow* glfwWindow;

	GLInputEngine::GLInputEngine()
	{
		LINA_CORE_TRACE("[Constructor] -> GLInputEngine ({0})", typeid(*this).name());
	}

	GLInputEngine::~GLInputEngine()
	{
		LINA_CORE_TRACE("[Destructor] -> GLInputEngine ({0})", typeid(*this).name());
	}

	void GLInputEngine::Initialize_Impl(void* contextWindowPointer)
	{
		LINA_CORE_TRACE("[Initialization] -> GLInputEngine ({0})", typeid(*this).name());
		glfwWindow = static_cast<GLFWwindow*>(contextWindowPointer);
	}

	void GLInputEngine::Tick_Impl()
	{
		glfwPollEvents();
	}

	bool GLInputEngine::GetKey_Impl(int keycode)
	{
		int state = glfwGetKey(glfwWindow, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool GLInputEngine::GetKeyDown_Impl(int keyCode)
	{
		static int* oldState = new int[NUM_KEY_STATES];
		int newState = glfwGetKey(glfwWindow, keyCode);
		bool flag = (newState == GLFW_PRESS && oldState[keyCode] == GLFW_RELEASE) ? true : false;
		oldState[keyCode] = newState;
		return flag;
	}
	bool GLInputEngine::GetKeyUp_Impl(int keyCode)
	{
		static int* oldState = new int[NUM_KEY_STATES];
		int newState = glfwGetKey(glfwWindow, keyCode);
		bool flag = (newState == GLFW_RELEASE && oldState[keyCode] == GLFW_PRESS) ? true : false;
		oldState[keyCode] = newState;
		return flag;
	}
	bool GLInputEngine::GetMouseButton_Impl(int button)
	{
		int state = glfwGetMouseButton(glfwWindow, button);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	bool GLInputEngine::GetMouseButtonDown_Impl(int button)
	{
		static int* oldState = new int[NUM_MOUSE_STATES];
		int newState = glfwGetMouseButton(glfwWindow, button);
		bool flag = (newState == GLFW_PRESS && oldState[button] == GLFW_RELEASE) ? true : false;
		oldState[button] = newState;
		return flag;
	}
	bool GLInputEngine::GetMouseButtonUp_Impl(int button)
	{
		static int* oldState = new int[NUM_MOUSE_STATES];
		int newState = glfwGetMouseButton(glfwWindow, button);
		bool flag = (newState == GLFW_RELEASE && oldState[button] == GLFW_PRESS) ? true : false;
		oldState[button] = newState;
		return flag;
	}

	Vector2F GLInputEngine::GetRawMouseAxis_Impl()
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

	Vector2F GLInputEngine::GetMouseAxis_Impl()
	{
		// TODO: This returns the raw mouse axis for now, smooth the raw & map between -1 & 1 later.
		return GetRawMouseAxis();
	}
	
	Vector2F GLInputEngine::GetMousePosition_Impl()
	{
		double xpos, ypos;
		glfwGetCursorPos(glfwWindow, &xpos, &ypos);
		return Vector2F(xpos, ypos);
	}


	void GLInputEngine::SetCursor_Impl(bool visible) const
	{
		if (visible)
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}

	void GLInputEngine::SetMousePosition_Impl(const Vector2F & v) const
	{
		glfwSetCursorPos(glfwWindow, v.GetX(), v.GetY());
	}
}

