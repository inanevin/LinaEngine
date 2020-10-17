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

#include "PackageManager/OpenGL/GLInputDevice.hpp"  
#include "Utility/Math/Math.hpp"
#include "Utility/Log.hpp"
#include <GLFW/glfw3.h>

namespace LinaEngine::Input
{

	static const float s_axisSensitivity = 0.1f;
	static const float s_mouseSensitivity = 5.0f;
	GLFWwindow* glfwWindow;

	GLInputDevice::GLInputDevice()
	{
		LINA_CORE_TRACE("[Constructor] -> GLInputDevice ({0})", typeid(*this).name());
	}

	GLInputDevice::~GLInputDevice()
	{
		LINA_CORE_TRACE("[Destructor] -> GLInputDevice ({0})", typeid(*this).name());
	}

	void GLInputDevice::Initialize(void* contextWindowPointer)
	{
		LINA_CORE_TRACE("[Initialization] -> GLInputDevice ({0})", typeid(*this).name());
		glfwWindow = static_cast<GLFWwindow*>(contextWindowPointer);
	}

	void GLInputDevice::Tick()
	{
		glfwPollEvents();
	}

	bool GLInputDevice::GetKey(int keycode)
	{
		int state = glfwGetKey(glfwWindow, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool GLInputDevice::GetKeyDown(int keyCode)
	{
		static int* oldState = new int[NUM_KEY_STATES];
		int newState = glfwGetKey(glfwWindow, keyCode);
		bool flag = (newState == GLFW_PRESS && oldState[keyCode] == GLFW_RELEASE) ? true : false;
		oldState[keyCode] = newState;
		return flag;
	}
	bool GLInputDevice::GetKeyUp(int keyCode)
	{
		static int* oldState = new int[NUM_KEY_STATES];
		int newState = glfwGetKey(glfwWindow, keyCode);
		bool flag = (newState == GLFW_RELEASE && oldState[keyCode] == GLFW_PRESS) ? true : false;
		oldState[keyCode] = newState;
		return flag;
	}
	bool GLInputDevice::GetMouseButton(int button)
	{
		int state = glfwGetMouseButton(glfwWindow, button);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	bool GLInputDevice::GetMouseButtonDown(int button)
	{
		static int* oldState = new int[NUM_MOUSE_STATES];
		int newState = glfwGetMouseButton(glfwWindow, button);
		bool flag = (newState == GLFW_PRESS && oldState[button] == GLFW_RELEASE) ? true : false;
		oldState[button] = newState;
		return flag;
	}
	bool GLInputDevice::GetMouseButtonUp(int button)
	{
		static int* oldState = new int[NUM_MOUSE_STATES];
		int newState = glfwGetMouseButton(glfwWindow, button);
		bool flag = (newState == GLFW_RELEASE && oldState[button] == GLFW_PRESS) ? true : false;
		oldState[button] = newState;
		return flag;
	}

	Vector2 GLInputDevice::GetRawMouseAxis()
	{
		// Storage for previous mouse position.
		static Vector2 oldMousePosition;

		// Get the cursor position.
		double posX, posY;
		glfwGetCursorPos(glfwWindow, &posX, &posY);

		// Get the delta mouse position.
		Vector2 currentMousePosition = Vector2((float)posX, (float)posY);
		Vector2 diff = currentMousePosition - oldMousePosition;
		Vector2 raw = Vector2::Zero;

		// Set raw axis values depending on the direction of the axis.
		if (diff.x > 0.0f) raw.x = 1.0f;
		else if (diff.x < 0.0f) raw.x = -1.0f;
		if (diff.y > 0) raw.y = 1.0f;
		else if (diff.y < 0) raw.y = -1.0f;

		// Set previous position.
		oldMousePosition = currentMousePosition;

		// Return raw.
		return raw;
	}

	Vector2 GLInputDevice::GetMouseAxis()
	{
		static Vector2 oldMousePos;

		double posX, posY;
		glfwGetCursorPos(glfwWindow, &posX, &posY);

		// Delta
		Vector2 diff = Vector2((float)(posX - oldMousePos.x), (float)(posY - oldMousePos.y));

		// Clamp and remap delta mouse position.
		diff.x = Math::Clamp(diff.x, -s_mouseSensitivity, s_mouseSensitivity);
		diff.y = Math::Clamp(diff.y, -s_mouseSensitivity, s_mouseSensitivity);
		diff.x = Math::Remap(diff.x, -s_mouseSensitivity, s_mouseSensitivity, -1.0f, 1.0f);
		diff.y = Math::Remap(diff.y, -s_mouseSensitivity, s_mouseSensitivity, -1.0f, 1.0f);
	
		// Set the previous position.
		oldMousePos = Vector2((float)posX, (float)posY);

		return diff;
	}

	Vector2 GLInputDevice::GetMousePosition()
	{
		double xpos, ypos;
		glfwGetCursorPos(glfwWindow, &xpos, &ypos);
		return Vector2((float)xpos, (float)ypos);
	}


	void GLInputDevice::SetCursorMode(CursorMode mode) const
	{
		switch (mode)
		{
		case CursorMode::Visible:
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			break;

		case CursorMode::Hidden:
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			break;

		case CursorMode::Disabled:
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			break;
		}
	}

	void GLInputDevice::SetMousePosition(const Vector2 & v) const
	{
		glfwSetCursorPos(glfwWindow, v.x, v.y);
	}
}

