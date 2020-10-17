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

/*
Class: GLWindow

Responsible for all window operations of an Open GL window context.

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