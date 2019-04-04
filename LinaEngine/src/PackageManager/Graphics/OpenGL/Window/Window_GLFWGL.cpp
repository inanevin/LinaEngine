/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Window_GLFWGL
Timestamp: 2/25/2019 9:20:33 AM

*/

#include "LinaPch.hpp"
#include "Window_GLFWGL.hpp"  
#include "Lina/Application.hpp"
#include "Lina/Events/ApplicationEvent.hpp"
#include "Lina/Events/KeyEvent.hpp"
#include "Lina/Events/MouseEvent.hpp"
#include "Lina/Input/InputEngine.hpp"

namespace LinaEngine
{

	Window* Window_GLFWGL::Create(const WindowProps& props)
	{
		LINA_CORE_INFO("GLFWGL Window Created!");
		return new Window_GLFWGL(props);
	}

	Window_GLFWGL::Window_GLFWGL() : Window::Window()
	{
		
	}

	Window_GLFWGL::Window_GLFWGL(const WindowProps & props) : Window::Window(props)
	{
		
	}

	Window_GLFWGL::~Window_GLFWGL()
	{
		Terminate();
	}

	void Window_GLFWGL::OnUpdate()
	{
		glfwPollEvents();

		if (!glfwWindowShouldClose(m_Window))
		{
			// Swap Buffers
			glfwSwapBuffers(m_Window);

		}
		else
		{
			// Notify listeners.
			m_WindowProps.EventCallback(WindowCloseEvent());
		}
	
	}

	void Window_GLFWGL::SetVSync(bool enabled)
	{
		glfwSwapInterval(enabled);
	}

	bool Window_GLFWGL::IsVSync() const
	{
		return false;
	}

	void Window_GLFWGL::SetMousePosition(const Vector2F &)
	{

	}

	static void GLFWErrorCallback(int error, const char* desc)
	{
		LINA_CORE_ERR("GLFW Error ({0}): {1}", error, desc);
	}

	void Window_GLFWGL::Initialize()
	{

		// Get input engine.
		inputEngine = &(Application::Get().GetInputEngine());

		// Initialize glfw & set window hints
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		// Create window
		m_Window = glfwCreateWindow(m_WindowProps.Width, m_WindowProps.Height, m_WindowProps.Title.c_str(), NULL, NULL);

		// Assert window creation.
		LINA_CORE_ENSURE_ASSERTFUNC(m_Window != NULL, , Terminate();, "GLFW Window is null!");

		// Set error callback
		glfwSetErrorCallback(GLFWErrorCallback);

		// Set window context initialized.
		m_IsInitialized = true;

		// Set context.
		glfwMakeContextCurrent(m_Window);

		// Load glad
		LINA_CORE_ENSURE_ASSERTFUNC(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), , , "GLAD Loader failed!");

		// Update OpenGL about the window data.
		glViewport(0, 0, m_WindowProps.Width, m_WindowProps.Height);

		// set user pointer for callbacks.
		glfwSetWindowUserPointer(m_Window, this);

		auto windowResizeFunc = [](GLFWwindow* w, int wi, int he)
		{
			static_cast<Window_GLFWGL*>(glfwGetWindowUserPointer(w))->WindowResized(w, wi, he);
		};

		auto windowCloseFunc = [](GLFWwindow* w)
		{
			static_cast<Window_GLFWGL*>(glfwGetWindowUserPointer(w))->WindowClosed(w);
		};

		auto windowKeyFunc = [](GLFWwindow* w, int k, int s, int a, int m)
		{
			static_cast<Window_GLFWGL*>(glfwGetWindowUserPointer(w))->WindowKeyCallback(w, k, s, a, m);
		};

		auto windowButtonFunc = [](GLFWwindow* w, int b, int a, int m)
		{
			static_cast<Window_GLFWGL*>(glfwGetWindowUserPointer(w))->WindowMouseButtonCallback(w, b, a, m);
		};

		auto windowMouseScrollFunc = [](GLFWwindow* w, double xOff, double yOff)
		{
			static_cast<Window_GLFWGL*>(glfwGetWindowUserPointer(w))->WindowMouseScrollCallback(w, xOff, yOff);
		};

		auto windowCursorPosFunc = [](GLFWwindow* w, double xPos, double yPos)
		{
			static_cast<Window_GLFWGL*>(glfwGetWindowUserPointer(w))->WindowCursorPosCallback(w, xPos, yPos);
		};

		auto windowFocusFunc = [](GLFWwindow* w, int f)
		{
			static_cast<Window_GLFWGL*>(glfwGetWindowUserPointer(w))->WindowFocusCallback(w, f);
		};

		auto charFunc = [](GLFWwindow* w, unsigned int k)
		{
			static_cast<Window_GLFWGL*>(glfwGetWindowUserPointer(w))->CharCallback(w, k);
		};



		auto keyPressedFunc = [](GLFWwindow* w, int key, int scancode, int action, int mods)
		{
			static_cast<Window_GLFWGL*>(glfwGetWindowUserPointer(w))->KeyCallback(w, key, scancode, action, mods);
		};

		auto mousePressedFunc = [](GLFWwindow* w, int button, int action, int mods)
		{
			static_cast<Window_GLFWGL*>(glfwGetWindowUserPointer(w))->MouseCallback(w, button, action, mods);
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

		SetVSync(true);
	}

	void Window_GLFWGL::Terminate()
	{
		glfwTerminate();
	}


	void Window_GLFWGL::WindowResized(GLFWwindow * window, int width, int height)
	{
		m_WindowProps.Width = width;
		m_WindowProps.Height = height;

		// Notify listeners.
		m_WindowProps.EventCallback(WindowResizeEvent(m_WindowProps.Width, m_WindowProps.Height));
	}

	void Window_GLFWGL::WindowClosed(GLFWwindow* window)
	{
		m_WindowProps.EventCallback(WindowCloseEvent());
	}

	void Window_GLFWGL::WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		switch (action)
		{
		case GLFW_PRESS:
			m_WindowProps.EventCallback(KeyPressedEvent(key, 0));
			break;
		case GLFW_RELEASE:
			m_WindowProps.EventCallback(KeyReleasedEvent(key));
			break;

		case GLFW_REPEAT:
			m_WindowProps.EventCallback(KeyPressedEvent(key, 1));
			break;
		}
	}

	void Window_GLFWGL::WindowMouseButtonCallback(GLFWwindow * window, int button, int action, int modes)
	{
		switch (action)
		{
		case GLFW_PRESS:
			m_WindowProps.EventCallback(MouseButtonPressedEvent(button));
			break;

		case GLFW_RELEASE:
			m_WindowProps.EventCallback(MouseButtonReleasedEvent(button));
			break;
		}
	}

	void Window_GLFWGL::WindowMouseScrollCallback(GLFWwindow * window, double xOff, double yOff)
	{
		m_WindowProps.EventCallback(MouseScrolledEvent((float)xOff, (float)yOff));
	}

	void Window_GLFWGL::WindowCursorPosCallback(GLFWwindow * window, double xPos, double yPos)
	{
		m_WindowProps.EventCallback(MouseMovedEvent((float)xPos, (float)yPos));
	}

	void Window_GLFWGL::WindowFocusCallback(GLFWwindow * window, int focused)
	{
		if (focused) m_WindowProps.EventCallback(WindowFocusEvent());
		else
			m_WindowProps.EventCallback(WindowFocusLostEvent());
	}

	void Window_GLFWGL::CharCallback(GLFWwindow * window, unsigned int keycode)
	{
		m_WindowProps.EventCallback(KeyTypedEvent(keycode));
	}

	void Window_GLFWGL::KeyCallback(GLFWwindow * w, int key, int scancode, int action, int mods)
	{
		inputEngine->DispatchKeyAction(key, action);
	}

	void Window_GLFWGL::MouseCallback(GLFWwindow * w, int button, int action, int mods)
	{
		inputEngine->DispatchMouseAction(button, action);
	}


}

