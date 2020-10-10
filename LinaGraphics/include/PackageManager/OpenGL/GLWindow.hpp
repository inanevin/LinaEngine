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

#include "Rendering/RenderingCommon.hpp"
#include "Rendering/Window.hpp"

struct GLFWwindow;


namespace LinaEngine::Graphics
{

	class GLWindow : public Window
	{
	public:

		GLWindow();
		~GLWindow();

		// Creates the native window.
		bool CreateContext(WindowProperties propsIn) override;

		// Called every frame.
		void Tick() override;

		// Enables/Disables Vsync.
		void SetVsync(bool enable) override;

		// Gets the native glfw window.
		virtual void* GetNativeWindow() const { return m_window; }

		// Returns window active time
		virtual double GetTime() override;

		// Resizes the native window
		virtual void SetSize(const Vector2& newSize) override;

		// Sets a new pos for the native window.
		virtual void SetPos(const Vector2& newPos) override;

		// Sets a new post for the native window based on monitor & screen center.
		virtual void SetPosCentered(const Vector2 newPos) override;

		// Minimizes the window.
		virtual void Iconify() override;

		// Maximizes the window.
		virtual void Maximize() override;

		// Closes the application.
		virtual void Close() override;

	private:


		/* Callbacks */
		void WindowResized(void* window, int width, int height);
		void WindowClosed(void* window);
		void WindowKeyCallback(void* window, int key, int scancode, int action, int mods);
		void WindowMouseButtonCallback(void* window, int button, int action, int modes);
		void WindowMouseScrollCallback(void* window, double xOff, double yOff);
		void WindowCursorPosCallback(void* window, double xPos, double yPos);
		void WindowFocusCallback(void* window, int focused);
		void CharCallback(void* window, unsigned int keycode);
		void KeyCallback(void* w, int key, int scancode, int action, int mods);
		void MouseCallback(void* w, int button, int action, int mods);

	private:


		void* m_window = nullptr;
		GLFWwindow* m_glfwWindow = nullptr;


	};
}


#endif