/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Ýnan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: SDLWindow
Timestamp: 12/31/2018 2:05:56 AM

*/

#pragma once

#include "LinaPch.hpp"

#ifdef LLF_GRAPHICS_SDLOpenGL

#include "SDLOpenGLWindow.hpp"  

namespace LinaEngine
{
	static bool isSDLInitialized = false;

	static void SDLErrorCallback(const char* description)
	{
		LINA_CORE_ERR("SDL Error: {1}", description);
	}

	Window* SDLOpenGLWindow::Create(const WindowProps& props)
	{
		LINA_CORE_INFO("SDL Window Created!");
		return new SDLOpenGLWindow(props);
	}

	SDLOpenGLWindow::SDLOpenGLWindow(const WindowProps& props)
	{
		Init(props);
	}

	SDLOpenGLWindow::~SDLOpenGLWindow()
	{
		Shutdown();
	}

	void SDLOpenGLWindow::Init(const WindowProps& props)
	{
	
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		LINA_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);
		
		// Initialize SDL, assert err check.
		if (!isSDLInitialized)
		{
			int status = SDL_Init(SDL_INIT_VIDEO);
			if (status!=0) SDLErrorCallback(SDL_GetError());
			LINA_CORE_ASSERT(status, "Could not initialize SDL!");
			isSDLInitialized = true;
		}

		// Set additional parameters for SDL window using SDL_WINDOW_OPENGL
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);	// 8 bits (at least) -> 2 to the pow of 8 amount of color data. 256.
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);	// 8 bits -> 2 to the pow of 8 amount of color data. 256.
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);	// 8 bits -> 2 to the pow of 8 amount of color data. 256.
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);	// 8 bits -> 2 to the pow of 8 amount of color data. 256.
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);	// How much data will SDL allocate for a single pixel.
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);	// Alloc an area for 2 blocks of display mem.

		// Create window
		m_Window = SDL_CreateWindow(m_Data.Title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_Data.Width, m_Data.Height, SDL_WINDOW_OPENGL);

		// We create a context using our window, so we will have power over our window via OpenGL -> GPU.
		m_GLContext = SDL_GL_CreateContext(m_Window);
		
		// Enable VSync
		SetVSync(true);

		
		/*
		
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.EventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});*/
	}



	void SDLOpenGLWindow::Shutdown()
	{
		// Deallocate GL context and window. (m_Window pointer is deleted via SDL_DestroyWindow already so no need to use delete again on that.)
		SDL_GL_DeleteContext(m_GLContext);
		SDL_DestroyWindow(m_Window);
	}

	void SDLOpenGLWindow::OnUpdate()
	{
		// Pumo Events
		SDL_PumpEvents();

		// Swap Buffers
		SDL_GL_SwapWindow(m_Window);
	}

	void SDLOpenGLWindow::SetVSync(bool enabled)
	{
		if (enabled)
			SDL_GL_SetSwapInterval(1);
		else
			SDL_GL_SetSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool SDLOpenGLWindow::IsVSync() const
	{
		return false;
		//return m_Data.VSync;
	}	

	/*

	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		HZ_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized)
		{
			// TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			HZ_CORE_ASSERT(success, "Could not intialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.EventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}	*/

}

#endif

