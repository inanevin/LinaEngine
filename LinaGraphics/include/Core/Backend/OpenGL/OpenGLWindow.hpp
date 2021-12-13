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

#ifndef OpenGLWindow_HPP
#define OpenGLWindow_HPP

#include "Rendering/RenderingCommon.hpp"

struct GLFWwindow;

namespace Lina
{
	class Application;
}

namespace Lina::Graphics
{

	class OpenGLWindow
	{
	public:

		OpenGLWindow();
		~OpenGLWindow();

		static OpenGLWindow* Get() { return s_openglWindow; }
		virtual void* GetNativeWindow() const { return m_window; }

		Vector2 GetSize() { return Vector2(m_windowProperties.m_width, m_windowProperties.m_height); }
		Vector2 GetPos() { return Vector2(m_windowProperties.m_xPos, m_windowProperties.m_yPos); }

		float GetWidth() { return m_windowProperties.m_width; }
		float GetHeight() { return m_windowProperties.m_height; }
		const WindowProperties& GetProperties() const { return m_windowProperties; }

		double GetTime();
		void SetVsync(int interval);
		void SetSize(const Vector2& newSize);
		void SetPos(const Vector2& newPos);
		void SetPosCentered(const Vector2 newPos);
		void Iconify();
		void Maximize();
		void Close();
		void Sleep(int milliseconds);
		void Tick();

	private:

		friend class Lina::Application;

		bool CreateContext(WindowProperties propsIn);

	private:

		static OpenGLWindow* s_openglWindow;
		WindowProperties m_windowProperties = WindowProperties();
		void* m_window = nullptr;
		GLFWwindow* m_glfwWindow = nullptr;
	};
}


#endif