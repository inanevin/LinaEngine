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
#include "Lina/Events/ApplicationEvent.hpp"


namespace LinaEngine
{

	Window* Window_GLFWGL::Create(const WindowProps& props)
	{
		LINA_CORE_INFO("SDL Window Created!");
		return new Window_GLFWGL(props);
	}

	Window_GLFWGL::Window_GLFWGL() : Window::Window()
	{
		Init();
	}

	Window_GLFWGL::Window_GLFWGL(const WindowProps & props) : Window::Window(props)
	{
		Init();
	}

	Window_GLFWGL::~Window_GLFWGL()
	{
		Terminate();
	}

	void Window_GLFWGL::OnUpdate()
	{
		if (!glfwWindowShouldClose(m_Window))
		{
			// Swap Buffers
			glfwSwapBuffers(m_Window);

			// Poll Events
			glfwPollEvents();
		}
		else
		{
			// Notify listeners.
			m_WindowProps.EventCallback(WindowCloseEvent());
		}
	
	}

	void Window_GLFWGL::SetVSync(bool enabled)
	{

	}

	bool Window_GLFWGL::IsVSync() const
	{
		return false;
	}

	void Window_GLFWGL::SetMousePosition(const Vector2F &)
	{

	}

	void Window_GLFWGL::Init()
	{
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

		auto func = [](GLFWwindow* w, int wi, int he)
		{
			static_cast<Window_GLFWGL*>(glfwGetWindowUserPointer(w))->WindowResized(w, wi, he);
		};

	
		// Register window resize callback.
		glfwSetFramebufferSizeCallback(m_Window, func);
	}

	void Window_GLFWGL::Terminate()
	{
		glfwTerminate();
	}


	void Window_GLFWGL::WindowResized(GLFWwindow * window, int width, int height)
	{
		glViewport(0, 0, width, height);

		// Notify listeners.
		m_WindowProps.EventCallback(WindowResizeEvent(m_WindowProps.Width, m_WindowProps.Height));
	}


}

