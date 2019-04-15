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

#pragma once

#ifndef GLWindow_HPP
#define GLWindow_HPP

#include "Lina/Rendering/Window.hpp"
#include <../vendor/GLFW/include/GLFW/glfw3.h>


namespace LinaEngine
{
	class GLWindow : public Window<GLWindow>
	{
	public:

		GLWindow(const WindowProperties& props = WindowProperties());
		~GLWindow();

		/* Creates the native window. */
		void Initialize_Impl();

		/* Called every frame */
		void Tick_Impl();

		/* Enables/Disables Vsync */
		void SetVsync_Impl(bool enable);

		/* Gets the native glfw window. */
		FORCEINLINE void* GetNativeWindow_Impl() const { return m_Window; }


	private:

		/* Callbacks */
		void WindowResized(GLFWwindow* window, int width, int height);
		void WindowClosed(GLFWwindow* window);
		void WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void WindowMouseButtonCallback(GLFWwindow* window, int button, int action, int modes);
		void WindowMouseScrollCallback(GLFWwindow* window, double xOff, double yOff);
		void WindowCursorPosCallback(GLFWwindow* window, double xPos, double yPos);
		void WindowFocusCallback(GLFWwindow* window, int focused);
		void CharCallback(GLFWwindow* window, unsigned int keycode);
		void KeyCallback(GLFWwindow* w, int key, int scancode, int action, int mods);
		void MouseCallback(GLFWwindow* w, int button, int action, int mods);

		GLFWwindow* m_Window = NULL;

	};
}


#endif