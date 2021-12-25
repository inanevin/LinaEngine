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

#include "Core/Backend/OpenGL/OpenGLWindow.hpp"
#include "Core/CommonInput.hpp"
#include "EventSystem/WindowEvents.hpp"
#include "EventSystem/InputEvents.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Log/Log.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"


namespace Lina::Graphics
{
	OpenGLWindow* OpenGLWindow::s_openglWindow = nullptr;

	void OpenGLWindow::Tick()
	{
		if (!glfwWindowShouldClose(m_glfwWindow))
			glfwSwapBuffers(m_glfwWindow);
	}

	static void GLFWErrorCallback(int error, const char* desc)
	{
		LINA_ERR("GLFW Error: {0} Description: {1} ", error, desc);
	}

	bool OpenGLWindow::CreateContext(ApplicationInfo& appInfo)
	{
		LINA_TRACE("[Initialization] -> OpenGLWindow ({0})", typeid(*this).name());

		// Set props.
		m_windowProperties = appInfo.m_windowProperties;

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
			LINA_ERR("GLFW could not initialize!");
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
			LINA_ERR("GLAD Loader failed!");
			return false;
		}

		if (appInfo.m_appMode == ApplicationMode::Editor)
		{
			int xpos, ypos, width, height;
			glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &xpos, &ypos, &width, &height);
			glfwSetWindowSizeLimits(m_glfwWindow, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE, height);
			m_windowProperties.m_workingAreaWidth = width;
			m_windowProperties.m_workingAreaHeight = height;
			SetPos(Vector2ui(0,0));
			SetSize(Vector2ui(width, height));
		}

		// Update OpenGL about the window data.
		glViewport(0, 0, m_windowProperties.m_width, m_windowProperties.m_height);

		SetVsync(0);

		// set user pointer for callbacks.
		glfwSetWindowUserPointer(m_glfwWindow, this);

		auto windowResizeFunc = [](GLFWwindow* w, int wi, int he)
		{
			auto* window = static_cast<OpenGLWindow*>(glfwGetWindowUserPointer(w));
			window->m_windowProperties.m_width = wi;
			window->m_windowProperties.m_height = he;
			Event::EventSystem::Get()->Trigger<Event::EWindowResized>(Event::EWindowResized{ window->m_window, window->m_windowProperties });
		};

		auto windowCloseFunc = [](GLFWwindow* w)
		{
			auto* window = static_cast<OpenGLWindow*>(glfwGetWindowUserPointer(w));
			window->Close();
		};

		auto windowKeyFunc = [](GLFWwindow* w, int key, int scancode, int action, int modes)
		{
			auto* window = static_cast<OpenGLWindow*>(glfwGetWindowUserPointer(w));

			Lina::Input::InputAction inputAction = Lina::Input::InputAction::Pressed;
			if (action == GLFW_RELEASE)
				inputAction = Lina::Input::InputAction::Released;
			else if (action == GLFW_REPEAT)
				inputAction = Lina::Input::InputAction::Repeated;

 			Event::EventSystem::Get()->Trigger<Event::EKeyCallback>(Event::EKeyCallback{ window->m_window, key, scancode, inputAction, modes });
		};

		auto windowButtonFunc = [](GLFWwindow* w, int button, int action, int modes)
		{
			auto* window = static_cast<OpenGLWindow*>(glfwGetWindowUserPointer(w));

			Lina::Input::InputAction inputAction = Lina::Input::InputAction::Pressed;
			if (action == GLFW_RELEASE)
				inputAction = Lina::Input::InputAction::Released;
			else if (action == GLFW_REPEAT)
				inputAction = Lina::Input::InputAction::Repeated;

			Event::EventSystem::Get()->Trigger<Event::EMouseButtonCallback>(Event::EMouseButtonCallback{ window->m_window, button, inputAction, modes });
		};

		auto windowMouseScrollFunc = [](GLFWwindow* w, double xOff, double yOff)
		{
			auto* window = static_cast<OpenGLWindow*>(glfwGetWindowUserPointer(w));
			Event::EventSystem::Get()->Trigger<Event::EMouseScrollCallback>(Event::EMouseScrollCallback{ window->m_window, xOff, yOff });
		};

		auto windowCursorPosFunc = [](GLFWwindow* w, double xPos, double yPos)
		{
			auto* window = static_cast<OpenGLWindow*>(glfwGetWindowUserPointer(w));
			Event::EventSystem::Get()->Trigger<Event::EMouseCursorCallback>(Event::EMouseCursorCallback{ window->m_window, xPos, yPos });
		};

		auto windowFocusFunc = [](GLFWwindow* w, int f)
		{
			auto* window = static_cast<OpenGLWindow*>(glfwGetWindowUserPointer(w));
			Event::EventSystem::Get()->Trigger<Event::EWindowFocused>(Event::EWindowFocused{ window->m_window, f });
		};

		// Register window callbacks.
		glfwSetFramebufferSizeCallback(m_glfwWindow, windowResizeFunc);
		glfwSetWindowCloseCallback(m_glfwWindow, windowCloseFunc);
		glfwSetKeyCallback(m_glfwWindow, windowKeyFunc);
		glfwSetMouseButtonCallback(m_glfwWindow, windowButtonFunc);
		glfwSetScrollCallback(m_glfwWindow, windowMouseScrollFunc);
		glfwSetCursorPosCallback(m_glfwWindow, windowCursorPosFunc);
		glfwSetWindowFocusCallback(m_glfwWindow, windowFocusFunc);

		m_window = static_cast<void*>(m_glfwWindow);
		Event::EventSystem::Get()->Trigger<Event::EWindowContextCreated>(Event::EWindowContextCreated{ m_window });
		return true;
	}


	void OpenGLWindow::Shutdown()
	{
		LINA_TRACE("[Shutdown] -> OpenGLWindow ({0})", typeid(*this).name());

		glfwTerminate();
	};

	void OpenGLWindow::SetVsync(int interval)
	{
		m_windowProperties.m_vsync = interval;
		glfwSwapInterval(interval);
	}

	double OpenGLWindow::GetTime()
	{
		return glfwGetTime();
	}

	void OpenGLWindow::SetSize(const Vector2ui& newSize)
	{
		glfwSetWindowSize(m_glfwWindow, newSize.x, newSize.y);
		m_windowProperties.m_width = newSize.x;
		m_windowProperties.m_height = newSize.y;
	}

	void OpenGLWindow::SetPos(const Vector2ui& newPos)
	{
		m_windowProperties.m_xPos = newPos.x;
		m_windowProperties.m_yPos = newPos.y;
		glfwSetWindowPos(m_glfwWindow, newPos.x, newPos.y);
	}

	void OpenGLWindow::SetPosCentered(const Vector2ui newPos)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		SetPos(Lina::Vector2ui((int)((float)mode->width / 2.0f + (float)newPos.x - (float)m_windowProperties.m_width / 2.0f),
			(int)((float)mode->height / 2.0f + (float)newPos.y - (float)m_windowProperties.m_height / 2.0f)));
	}


	void OpenGLWindow::Iconify()
	{
		m_windowProperties.m_windowState = WindowState::Iconified;
		glfwIconifyWindow(m_glfwWindow);
	}

	void OpenGLWindow::Maximize()
	{
		if (m_windowProperties.m_windowState != WindowState::Maximized)
		{
			m_windowProperties.m_widthBeforeMaximize = m_windowProperties.m_width;
			m_windowProperties.m_heightBeforeMaximize = m_windowProperties.m_height;
			m_windowProperties.m_xPosBeforeMaximize = m_windowProperties.m_xPos;
			m_windowProperties.m_yPosBeforeMaximize = m_windowProperties.m_yPos;
			SetPos(Vector2ui(0,0));
			SetSize(Vector2ui(m_windowProperties.m_workingAreaWidth, m_windowProperties.m_workingAreaHeight));
			m_windowProperties.m_windowState = WindowState::Maximized;
			//glfwMaximizeWindow(m_glfwWindow);
		}
		else
		{
			SetPos(Vector2ui(m_windowProperties.m_xPosBeforeMaximize, m_windowProperties.m_yPosBeforeMaximize));
			SetSize(Vector2ui(m_windowProperties.m_widthBeforeMaximize, m_windowProperties.m_heightBeforeMaximize));
			m_windowProperties.m_windowState = WindowState::Normal;
			//glfwRestoreWindow(m_glfwWindow);
		}
	}

	void OpenGLWindow::Close()
	{
		Event::EventSystem::Get()->Trigger<Event::EWindowClosed>(Event::EWindowClosed{ m_window });
	}

	void OpenGLWindow::Sleep(int milliseconds)
	{
		glfwWaitEventsTimeout((float)milliseconds / 1000.0f);
	}


}

