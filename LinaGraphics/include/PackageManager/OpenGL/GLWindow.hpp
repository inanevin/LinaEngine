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
#include "Core/Common.hpp"
#include <functional>

namespace LinaEngine
{
	namespace Input
	{
		class InputEngine;
	}
}

namespace LinaEngine::Graphics
{

	class GLWindow
	{
	public:

		// Creates the native window.
		bool Initialize(WindowProperties& propsIn);

		// Called every frame.
		void Tick();

		// Enables/Disables Vsync.
		void SetVsync(bool enable);

		// Gets the native glfw window.
		FORCEINLINE void* GetNativeWindow() const { return m_Window; }

		// Sets event callbacks.
		//FORCEINLINE void SetEventCallback(const std::function<void(Event&)>& callback) { m_EventCallback = callback; }
		FORCEINLINE void SetKeyCallback(std::function<void(int,int)>& callback) { m_KeyCallback = callback; }
		FORCEINLINE void SetMouseCallback(std::function<void(int,int)>& callback) { m_MouseCallback = callback; }

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

		friend class Window;

		GLWindow();
		~GLWindow();
	
		void* m_Window = nullptr;
		WindowProperties* m_WindowProperties = nullptr;
		LinaEngine::Input::InputEngine* inputEngine = nullptr;

		std::function<void(int, int)> m_KeyCallback;
		std::function<void(int, int)> m_MouseCallback;

	};
}


#endif