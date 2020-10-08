/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: GLWindow
Timestamp: 4/14/2019 5:12:19 PM

*/

/*
// ** WINDOWS ONLY **
#define GLFW_EXPOSE_NATIVE_WIN32
HWND win32Window = glfwGetWin32Window(window);

long style = GetWindowLong(win32Window, GWL_STYLE);

//style |= WS_DLGFRAME;
//style &= ~WS_ACTIVECAPTION;
//style &= ~WS_CAPTION;
//style &= ~WS_SYSMENU;
//style |= WS_THICKFRAME;
// style |= WS_SIZEBOX;
//style |= WS_BORDER;
//SetWindowLong(win32Window, GWL_STYLE, style);
*/

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "PackageManager/OpenGL/GLWindow.hpp"
#include "Utility/Log.hpp"



namespace LinaEngine::Graphics
{
	GLWindow::GLWindow()
	{
		LINA_CORE_TRACE("[Constructor] -> GLWindow ({0})", typeid(*this).name());
		SetVsync(false);
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
		m_windowProperties.m_Width = newSize.x;
		m_windowProperties.m_Height = newSize.y;
	}

	void GLWindow::SetPos(const Vector2& newPos)
	{
		m_windowProperties.m_xPos = newPos.x;
		m_windowProperties.m_yPos = newPos.y;
		glfwSetWindowPos(m_glfwWindow, newPos.x, newPos.y);
	}
	static void GLFWErrorCallback(int error, const char* desc)
	{
		LINA_CORE_ERR("GLFW Error: {0} Description: {1} ", error, desc);
	}

	bool GLWindow::CreateContext(WindowProperties& propsIn)
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

		// Create window
		m_glfwWindow = (glfwCreateWindow(m_windowProperties.m_Width, m_windowProperties.m_Height, m_windowProperties.m_Title.c_str(), NULL, NULL));

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
	

		// Update OpenGL about the window data.
		glViewport(0, 0, m_windowProperties.m_Width, m_windowProperties.m_Height);

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


	void GLWindow::WindowResized(void* window, int width, int height)
	{
		m_windowProperties.m_Width = width;
		m_windowProperties.m_Height = height;		
		m_windowResizeCallback(Vector2((float)width, (float)height));
	}

	void GLWindow::WindowClosed(void* window)
	{
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

