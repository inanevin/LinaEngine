/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: Window
Timestamp: 12/31/2018 1:43:13 AM

*/

#pragma once


#ifndef Window_HPP
#define Window_HPP

#include "Lina/Events/Event.hpp"

namespace LinaEngine
{
	/* Struct containing basic data about window properties. */
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		bool VSync;

		WindowProps(const std::string& title = "Lina Engine",
			unsigned int width = 1280,
			unsigned int height = 720)
			: Title(title), Width(width), Height(height), VSync(false)
		{
		}

		std::function<void(Event&)> EventCallback;
	};

	// Interface representing a desktop system based Window
	class LINA_API Window
	{
	public:

		using EventCallbackFn = std::function<void(Event&)>;
		Window(const WindowProps& wp = WindowProps());
		virtual ~Window() {}

		/* Gets called when the window updates each frame. */
		virtual void OnUpdate() = 0;

		/* Returns absolute width & height of the window. */
		unsigned int GetWidth() const { return m_WindowProps.Width; }
		unsigned int GetHeight() const { return m_WindowProps.Height; }

		/* Sets a function pointer as a callback to the desired event.*/
		inline void SetEventCallback(const EventCallbackFn& callback) { m_WindowProps.EventCallback = callback; }

		/* Enables/Disables vertical sync. */
		virtual void SetVSync(bool enabled) = 0;

		/* Returns whether vsync is enabled or not. */
		virtual bool IsVSync() const = 0;

		/* Sets mouse to the desired coordinates in the screen space. */
		virtual void SetMousePosition(const Vector2F&) = 0;

	protected:

		WindowProps m_WindowProps;

		//static Window* Create(const WindowProps& props = WindowProps());
	};


}


#endif