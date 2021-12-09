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

#include "PackageManager/OpenGL/GLWindow.hpp"
#include "Utility/Log.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"


namespace LinaEngine::Graphics
{
	GLWindow::GLWindow()
	{
		LINA_CORE_TRACE("[Constructor] -> GLWindow ({0})", typeid(*this).name());
	
	}

	GLWindow::~GLWindow()
	{
		LINA_CORE_TRACE("[Destructor] -> GLWindow ({0})", typeid(*this).name());
		glfwTerminate();
	}

	void GLWindow::Tick()
	{
		if (!glfwWindowShouldClose(static_cast<GLFWwindow*>(m_window)))
		{
			// Swap Buffers
			glfwSwapBuffers(static_cast<GLFWwindow*>(m_window));
		}
		else
		{
			// Notify listeners.
			//m_EventCallback(WindowCloseEvent());
		}

	}

	static void GLFWErrorCallback(int error, const char* desc)
	{
		LINA_CORE_ERR("GLFW Error: {0} Description: {1} ", error, desc);
	}

	bool GLWindow::CreateContext(WindowProperties propsIn)
	{
		LINA_CORE_TRACE("[Initialization] -> GLWindow ({0})", typeid(*this).name());

		
		// Set props.
		m_windowProperties = propsIn;
				
		// Initialize glfw & set window hints
		int init = glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_DECORATED, m_windowProperties.m_decorated);
		glfwWindowHint(GLFW_RESIZABLE, m_windowProperties.m_resizable);

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
		if (m_windowProperties.m_fullscreen)
		{
			m_windowProperties.m_width = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
			m_windowProperties.m_height = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
		}


		// Build window
		m_glfwWindow = (glfwCreateWindow(m_windowProperties.m_width, m_windowProperties.m_height, m_windowProperties.m_title.c_str(), m_windowProperties.m_fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL));
		
		// Set window position.
		SetPosCentered(Vector2::Zero);

		if (!m_glfwWindow)
		{
			// Assert window creation.
			LINA_CORE_ERR("GLFW could not initialize!");
			return false;
		}
		// Set error callback
		glfwSetErrorCallback(GLFWErrorCallback);

		// Set context.
		glfwMakeContextCurrent(m_glfwWindow);
		// Load glad
		bool loaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (!loaded)
		{
			LINA_CORE_ERR("GLAD Loader failed!");
			return false;
		}

		int xpos, ypos, width, height;
		glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &xpos, &ypos, &width, &height);
		glfwSetWindowSizeLimits(m_glfwWindow, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE, height);
		m_windowProperties.m_workingAreaWidth = width;
		m_windowProperties.m_workingAreaHeight = height;
		SetPos(Vector2::Zero);
		SetSize(Vector2(width, height));

		// Update OpenGL about the window data.
		glViewport(0, 0, m_windowProperties.m_width, m_windowProperties.m_height);

		SetVsync(false);

		// set user pointer for callbacks.
		glfwSetWindowUserPointer(m_glfwWindow, this);

		auto windowResizeFunc = [](GLFWwindow* w, int wi, int he)
		{
			static_cast<GLWindow*>(glfwGetWindowUserPointer(w))->WindowResized(w, wi, he);
		};

		auto windowCloseFunc = [](GLFWwindow* w)
		{
			static_cast<GLWindow*>(glfwGetWindowUserPointer(w))->WindowClosed(w);
		};

		auto windowKeyFunc = [](GLFWwindow* w, int k, int s, int a, int m)
		{
			static_cast<GLWindow*>(glfwGetWindowUserPointer(w))->WindowKeyCallback(w, k, s, a, m);
		};

		auto windowButtonFunc = [](GLFWwindow* w, int b, int a, int m)
		{
			static_cast<GLWindow*>(glfwGetWindowUserPointer(w))->WindowMouseButtonCallback(w, b, a, m);
		};

		auto windowMouseScrollFunc = [](GLFWwindow* w, double xOff, double yOff)
		{
			static_cast<GLWindow*>(glfwGetWindowUserPointer(w))->WindowMouseScrollCallback(w, xOff, yOff);
		};

		auto windowCursorPosFunc = [](GLFWwindow* w, double xPos, double yPos)
		{
			static_cast<GLWindow*>(glfwGetWindowUserPointer(w))->WindowCursorPosCallback(w, xPos, yPos);
		};

		auto windowFocusFunc = [](GLFWwindow* w, int f)
		{
			static_cast<GLWindow*>(glfwGetWindowUserPointer(w))->WindowFocusCallback(w, f);
		};

		auto charFunc = [](GLFWwindow* w, unsigned int k)
		{
			static_cast<GLWindow*>(glfwGetWindowUserPointer(w))->CharCallback(w, k);
		};


		auto keyPressedFunc = [](GLFWwindow* w, int key, int scancode, int action, int mods)
		{
			static_cast<GLWindow*>(glfwGetWindowUserPointer(w))->KeyCallback(w, key, scancode, action, mods);
		};

		auto mousePressedFunc = [](GLFWwindow* w, int button, int action, int mods)
		{
			static_cast<GLWindow*>(glfwGetWindowUserPointer(w))->MouseCallback(w, button, action, mods);
		};


		// Register window callbacks.
		glfwSetFramebufferSizeCallback(m_glfwWindow, windowResizeFunc);
		glfwSetWindowCloseCallback(m_glfwWindow, windowCloseFunc);
		glfwSetKeyCallback(m_glfwWindow, windowKeyFunc);
		glfwSetMouseButtonCallback(m_glfwWindow, windowButtonFunc);
		glfwSetScrollCallback(m_glfwWindow, windowMouseScrollFunc);
		glfwSetCursorPosCallback(m_glfwWindow, windowCursorPosFunc);
		glfwSetWindowFocusCallback(m_glfwWindow, windowFocusFunc);
		glfwSetCharCallback(m_glfwWindow, charFunc);
		glfwSetKeyCallback(m_glfwWindow, keyPressedFunc);
		glfwSetMouseButtonCallback(m_glfwWindow, mousePressedFunc);

		m_window = static_cast<void*>(m_glfwWindow);
		return true;
	}



	void GLWindow::SetVsync(bool enabled)
	{
		Window::SetVsync(enabled);
		glfwSwapInterval(enabled);
	}

	double GLWindow::GetTime()
	{
		return glfwGetTime();
	}

	void GLWindow::SetSize(const Vector2& newSize)
	{
		glfwSetWindowSize(m_glfwWindow, newSize.x, newSize.y);
		m_windowProperties.m_width = newSize.x;
		m_windowProperties.m_height = newSize.y;
	}

	void GLWindow::SetPos(const Vector2& newPos)
	{
		m_windowProperties.m_xPos = newPos.x;
		m_windowProperties.m_yPos = newPos.y;
		glfwSetWindowPos(m_glfwWindow, newPos.x, newPos.y);
	}

	void GLWindow::SetPosCentered(const Vector2 newPos)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		SetPos(LinaEngine::Vector2(mode->width / 2.0f + newPos.x - m_windowProperties.m_width / 2.0f, mode->height / 2.0f + newPos.y - m_windowProperties.m_height / 2.0f));
	}


	void GLWindow::Iconify()
	{
		m_windowProperties.m_windowState = WindowState::Iconified;
		glfwIconifyWindow(m_glfwWindow);
	}

	void GLWindow::Maximize()
	{
		if (m_windowProperties.m_windowState != WindowState::Maximized)
		{
			m_windowProperties.m_widthBeforeMaximize = m_windowProperties.m_width;
			m_windowProperties.m_heightBeforeMaximize = m_windowProperties.m_height;
			m_windowProperties.m_xPosBeforeMaximize = m_windowProperties.m_xPos;
			m_windowProperties.m_yPosBeforeMaximize = m_windowProperties.m_yPos;
			SetPos(Vector2::Zero);
			SetSize(Vector2(m_windowProperties.m_workingAreaWidth, m_windowProperties.m_workingAreaHeight));
			m_windowProperties.m_windowState = WindowState::Maximized;
			//glfwMaximizeWindow(m_glfwWindow);
		}
		else
		{
			SetPos(Vector2(m_windowProperties.m_xPosBeforeMaximize, m_windowProperties.m_yPosBeforeMaximize));
			SetSize(Vector2(m_windowProperties.m_widthBeforeMaximize, m_windowProperties.m_heightBeforeMaximize));
			m_windowProperties.m_windowState = WindowState::Normal;
			//glfwRestoreWindow(m_glfwWindow);
		}
	}

	void GLWindow::Close()
	{
		WindowClosed(m_window);
	}

	void GLWindow::Sleep(int milliseconds)
	{
		glfwWaitEventsTimeout((float)milliseconds / 1000.0f);
	}


	void GLWindow::WindowResized(void* window, int width, int height)
	{
		m_windowProperties.m_width = width;
		m_windowProperties.m_height = height;
		if (m_windowResizeCallback)
			m_windowResizeCallback(Vector2((float)width, (float)height));
	}

	void GLWindow::WindowClosed(void* window)
	{
		if (m_windowCloseCallback)
			m_windowCloseCallback();
	}

	void GLWindow::WindowKeyCallback(void* window, int key, int scancode, int action, int mods)
	{
		switch (action)
		{
		case GLFW_PRESS:

			break;
		case GLFW_RELEASE:

			break;

		case GLFW_REPEAT:

			break;
		}
	}

	void GLWindow::WindowMouseButtonCallback(void* window, int button, int action, int modes)
	{
		switch (action)
		{
		case GLFW_PRESS:

			break;

		case GLFW_RELEASE:

			break;
		}
	}

	void GLWindow::WindowMouseScrollCallback(void* window, double xOff, double yOff)
	{

	}

	void GLWindow::WindowCursorPosCallback(void* window, double xPos, double yPos)
	{

	}

	void GLWindow::WindowFocusCallback(void* window, int focused)
	{

	}

	void GLWindow::CharCallback(void* window, unsigned int keycode)
	{
	}

	void GLWindow::KeyCallback(void* w, int key, int scancode, int action, int mods)
	{
		m_keyCallback(key, action);
	}

	void GLWindow::MouseCallback(void* w, int button, int action, int mods)
	{
		m_mouseCallback(button, action);
	}


}

