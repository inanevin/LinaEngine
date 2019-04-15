/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: GLInputDevice
Timestamp: 4/14/2019 5:15:15 PM

*/

#include "LinaPch.hpp"
#include "GLInputDevice.hpp"  
#include "GLFW/glfw3.h"



namespace LinaEngine
{
	static const float axisSensitivity = 0.1f;

	GLInputDevice::GLInputDevice()
	{
		LINA_CORE_TRACE("[Constructor] -> GLInputDevice ({0})", typeid(*this).name());
	}

	GLInputDevice::~GLInputDevice()
	{
		LINA_CORE_TRACE("[Destructor] -> GLInputDevice ({0})", typeid(*this).name());
	}

	void GLInputDevice::Initialize_Impl(const Window<PAMWindow>& window)
	{

		LINA_CORE_TRACE("[Initialization] -> GLInputDEvice ({0})", typeid(*this).name());

		glfwWindow = static_cast<GLFWwindow*>(window.GetNativeWindow());
		LINA_CORE_ENSURE_ASSERT(glfwWindow != NULL, , "GLFW Window is null!");

	}

	void GLInputDevice::Tick_Impl()
	{
		glfwPollEvents();
	}

	bool GLInputDevice::GetKey_Impl(int keycode)
	{
		int state = glfwGetKey(glfwWindow, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool GLInputDevice::GetKeyDown_Impl(int keyCode)
	{
		static int* oldState = new int[NUM_KEY_STATES];
		int newState = glfwGetKey(glfwWindow, keyCode);
		bool flag = (newState == GLFW_PRESS && oldState[keyCode] == GLFW_RELEASE) ? true : false;
		oldState[keyCode] = newState;
		return flag;
	}
	bool GLInputDevice::GetKeyUp_Impl(int keyCode)
	{
		static int* oldState = new int[NUM_KEY_STATES];
		int newState = glfwGetKey(glfwWindow, keyCode);
		bool flag = (newState == GLFW_RELEASE && oldState[keyCode] == GLFW_PRESS) ? true : false;
		oldState[keyCode] = newState;
		return flag;
	}
	bool GLInputDevice::GetMouseButton_Impl(int button)
	{
		int state = glfwGetMouseButton(glfwWindow, button);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	bool GLInputDevice::GetMouseButtonDown_Impl(int button)
	{
		static int* oldState = new int[NUM_MOUSE_STATES];
		int newState = glfwGetMouseButton(glfwWindow, button);
		bool flag = (newState == GLFW_PRESS && oldState[button] == GLFW_RELEASE) ? true : false;
		oldState[button] = newState;
		return flag;
	}
	bool GLInputDevice::GetMouseButtonUp_Impl(int button)
	{
		static int* oldState = new int[NUM_MOUSE_STATES];
		int newState = glfwGetMouseButton(glfwWindow, button);
		bool flag = (newState == GLFW_RELEASE && oldState[button] == GLFW_PRESS) ? true : false;
		oldState[button] = newState;
		return flag;
	}

	Vector2F GLInputDevice::GetRawMouseAxis_Impl()
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

	Vector2F GLInputDevice::GetMouseAxis_Impl()
	{
		// TODO: This returns the raw mouse axis for now, smooth the raw & map between -1 & 1 later.
		return GetRawMouseAxis();
	}
	
	Vector2F GLInputDevice::GetMousePosition_Impl()
	{
		double xpos, ypos;
		glfwGetCursorPos(glfwWindow, &xpos, &ypos);
		return Vector2F(xpos, ypos);
	}


	void GLInputDevice::SetCursor_Impl(bool visible) const
	{
		if (visible)
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}

	void GLInputDevice::SetMousePosition_Impl(const Vector2F & v) const
	{
		glfwSetCursorPos(glfwWindow, v.GetX(), v.GetY());
	}
}

