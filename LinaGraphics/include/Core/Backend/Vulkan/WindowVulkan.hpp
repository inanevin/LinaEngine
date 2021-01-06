/*
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

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

/*
Class: Window

Responsible for all window operations of an Open GL window context.

Timestamp: 4/14/2019 5:12:19 PM
*/

#pragma once

#ifndef WindowGLFW_HPP
#define WindowGLFW_HPP

#include "linagraphics_export.h"
#include "Core/Common.hpp"
#include "Math/Vector.hpp"
#include "ECS/ECS.hpp"
#include "EventSystem/Events.hpp"

#define GLFW_INCLUDE_NONE
#define GLFW_DLL
#include <vulkan/vulkan.h>
#include "GLFW/glfw3.h"

namespace Lina
{
	namespace Event
	{
		class EventSystem;
	}
}

namespace Lina::Graphics
{
	class WindowVulkan
	{
	public:

	private:

		friend class RenderEngineVulkan;
		
		WindowVulkan& operator=(const WindowVulkan&) = delete;
		WindowVulkan(const WindowVulkan&) = delete;
		WindowVulkan() {};
		~WindowVulkan();
		void SetReferences(Event::EventSystem* eventSys, WindowProperties& props);
		bool CreateContext();
		void Terminate();
		void WaitEvents();
		void OnWindowResize(int w, int h);
		bool CreateWindowSurface(VkInstance& instance, VkSurfaceKHR& surface);

	public:

		std::vector<const char*> GetRequiredExtensions();

		// Enables/Disables Vsync.
		void SetVsync(bool enable);

		// Gets the native glfw window.
		void* GetNativeWindow() const { return m_window; }
	
		// Resizes the native window
		void SetSize(const Vector2& newSize);

		// Sets a new pos for the native window.
		void SetPos(const Vector2& newPos);

		// Sets a new post for the native window based on monitor & screen center.
		void SetPosCentered(const Vector2 newPos);

		// Minimizes the window.
		void Iconify();

		// Maximizes the window.
		void Maximize();

	private:

		void OnPreMainLoop(Event::EPreMainLoop& e);
		void OnPostMainLoop(Event::EPostMainLoop& e);


	private:

		void* m_window = nullptr;
		GLFWwindow* m_glfwWindow = nullptr;
		WindowProperties m_windowProperties;
		Event::EventSystem* m_eventSys;
	};
}


#endif