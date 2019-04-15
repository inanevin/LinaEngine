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


namespace LinaEngine
{

	class GLWindow : public Window<GLWindow>
	{
	public:

		GLWindow(const WindowProperties& props = WindowProperties());
		~GLWindow();

		/* Creates the native window. */
		bool Initialize_Impl();

		/* Called every frame */
		void Tick_Impl();

		/* Enables/Disables Vsync */
		void SetVsync_Impl(bool enable);

		/* Gets the native glfw window. */
		FORCEINLINE void* GetNativeWindow_Impl() const { return m_Window; }


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

		void* m_Window = NULL;

	};
}


#endif