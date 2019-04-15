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

#include "LinaPch.hpp"

/************ GLAD NEEDS TO BE INCLUDED BEFORE OTHERS ************/
#include "glad/glad.h"
/************ GLAD NEEDS TO BE INCLUDED BEFORE OTHERS ************/

#include "GLWindow.hpp"  
#include "Lina/Core/Application.hpp"
#include "Lina/Events/ApplicationEvent.hpp"
#include "Lina/Events/KeyEvent.hpp"
#include "Lina/Events/MouseEvent.hpp"


namespace LinaEngine
{


	GLWindow::GLWindow(const WindowProperties& props) : Window(props)
	{
		LINA_CORE_TRACE("[Constructor] -> GLWindow ({0})", typeid(*this).name());
	}

	GLWindow::~GLWindow()
	{
		LINA_CORE_TRACE("[Destructor] -> GLWindow ({0})", typeid(*this).name());
		glfwTerminate();
	}

	void GLWindow::Tick_Impl()
	{
		if (!glfwWindowShouldClose(m_Window))
		{
			// Swap Buffers
			glfwSwapBuffers(m_Window);
		}
		else
		{
			// Notify listeners.
			m_EventCallback(WindowCloseEvent());
		}

	}

	void GLWindow::SetVsync_Impl(bool enabled)
	{
		glfwSwapInterval(enabled);
	}

	static void GLFWErrorCallback(int error, const char* desc)
	{
		LINA_CORE_ERR("GLFW Error ({0}): {1}", error, desc);
	}

	void GLWindow::Initialize_Impl()
	{
		LINA_CORE_TRACE("[Initialization] -> GLWindow ({0})", typeid(*this).name());

		// Initialize glfw & set window hints
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		// Create window
		m_Window = glfwCreateWindow(m_Properties.m_Width, m_Properties.m_Height, m_Properties.m_Title.c_str(), NULL, NULL);

		// Assert window creation.
		LINA_CORE_ASSERT(m_Window, "GLFW could not initialize!");

		// Set error callback
		glfwSetErrorCallback(GLFWErrorCallback);

		// Set context.
		glfwMakeContextCurrent(m_Window);

		// Load glad
		LINA_CORE_ENSURE_ASSERTFUNC(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), , , "GLAD Loader failed!");

		// Update OpenGL about the window data.
		glViewport(0, 0, m_Properties.m_Width, m_Properties.m_Height);

		// set user pointer for callbacks.
		glfwSetWindowUserPointer(m_Window, this);

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
		glfwSetFramebufferSizeCallback(m_Window, windowResizeFunc);
		glfwSetWindowCloseCallback(m_Window, windowCloseFunc);
		glfwSetKeyCallback(m_Window, windowKeyFunc);
		glfwSetMouseButtonCallback(m_Window, windowButtonFunc);
		glfwSetScrollCallback(m_Window, windowMouseScrollFunc);
		glfwSetCursorPosCallback(m_Window, windowCursorPosFunc);
		glfwSetWindowFocusCallback(m_Window, windowFocusFunc);
		glfwSetCharCallback(m_Window, charFunc);
		glfwSetKeyCallback(m_Window, keyPressedFunc);
		glfwSetMouseButtonCallback(m_Window, mousePressedFunc);
	}


	void GLWindow::WindowResized(GLFWwindow * window, int width, int height)
	{
		m_Properties.m_Width = width;
		m_Properties.m_Height = height;

		glViewport(0, 0, m_Properties.m_Width, m_Properties.m_Height);

		// Notify listeners.
		m_EventCallback(WindowResizeEvent(m_Properties.m_Width, m_Properties.m_Height));
	}

	void GLWindow::WindowClosed(GLFWwindow* window)
	{
		m_EventCallback(WindowCloseEvent());
	}

	void GLWindow::WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		switch (action)
		{
		case GLFW_PRESS:
			m_EventCallback(KeyPressedEvent(key, 0));
			break;
		case GLFW_RELEASE:
			m_EventCallback(KeyReleasedEvent(key));
			break;

		case GLFW_REPEAT:
			m_EventCallback(KeyPressedEvent(key, 1));
			break;
		}
	}

	void GLWindow::WindowMouseButtonCallback(GLFWwindow * window, int button, int action, int modes)
	{
		switch (action)
		{
		case GLFW_PRESS:
			m_EventCallback(MouseButtonPressedEvent(button));
			break;

		case GLFW_RELEASE:
			m_EventCallback(MouseButtonReleasedEvent(button));
			break;
		}
	}

	void GLWindow::WindowMouseScrollCallback(GLFWwindow * window, double xOff, double yOff)
	{
		m_EventCallback(MouseScrolledEvent((float)xOff, (float)yOff));
	}

	void GLWindow::WindowCursorPosCallback(GLFWwindow * window, double xPos, double yPos)
	{
		m_EventCallback(MouseMovedEvent((float)xPos, (float)yPos));
	}

	void GLWindow::WindowFocusCallback(GLFWwindow * window, int focused)
	{
		if (focused)
			m_EventCallback(WindowFocusEvent());
		else
			m_EventCallback(WindowFocusLostEvent());
	}

	void GLWindow::CharCallback(GLFWwindow * window, unsigned int keycode)
	{
		m_EventCallback(KeyTypedEvent(keycode));
	}

	void GLWindow::KeyCallback(GLFWwindow * w, int key, int scancode, int action, int mods)
	{
		//inputDevice->DispatchKeyAction(static_cast<Input::Key>(key), action);
		if (action == GLFW_PRESS)
			m_EventCallback(KeyPressedEvent(static_cast<Input::Key>(key), 0));
		else if (action == GLFW_REPEAT)
			m_EventCallback(KeyPressedEvent(static_cast<Input::Key>(key), 1));
		else if (action == GLFW_RELEASE)
			m_EventCallback(KeyReleasedEvent(static_cast<Input::Key>(key)));
	}

	void GLWindow::MouseCallback(GLFWwindow * w, int button, int action, int mods)
	{
		//inputDevice->DispatchMouseAction(static_cast<Input::Mouse>(button), action);
		if (action == GLFW_PRESS)
			m_EventCallback(MouseButtonPressedEvent(static_cast<Input::Mouse>(button)));
		else if (action == GLFW_RELEASE)
			m_EventCallback(MouseButtonReleasedEvent(static_cast<Input::Mouse>(button)));

	}

}

