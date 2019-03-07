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

#pragma once

#ifndef Window_GLFWGL_HPP
#define Window_GLFWGL_HPP

#include "Lina/Rendering/Window.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace LinaEngine
{
	class Window_GLFWGL : public Window
	{
	public:

		Window_GLFWGL();
		Window_GLFWGL(const WindowProps& props);
		~Window_GLFWGL();
		static Window* Create(const WindowProps& props = WindowProps());

		/* Gets called when the window updates each frame. */
		void OnUpdate() override;

		/* Returns whether GLFW window is initialized. */
		bool IsInitialized() { return m_IsInitialized; }

		/* Enables/Disables vertical sync. */
		virtual void SetVSync(bool enabled) override;

		/* Returns whether vsync is enabled or not. */
		virtual bool IsVSync() const override;

		/* Sets mouse to the desired coordinates in the screen space. */
		virtual void SetMousePosition(const Vector2F&) override;

		inline GLFWwindow* GetGLFWWindow() { return m_Window; }

	private:

		void Init();
		void Terminate();
		void WindowResized(GLFWwindow* window, int width, int height);
		void WindowClosed(GLFWwindow* window);
		void WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void WindowMouseButtonCallback(GLFWwindow* window, int button, int action, int modes);
		void WindowMouseScrollCallback(GLFWwindow* window, double xOff, double yOff);
		void WindowCursorPosCallback(GLFWwindow* window, double xPos, double yPos);
		void WindowFocusCallback(GLFWwindow* window, int focused);
		bool m_IsInitialized = false;
		GLFWwindow* m_Window = NULL;
	};
}


#endif