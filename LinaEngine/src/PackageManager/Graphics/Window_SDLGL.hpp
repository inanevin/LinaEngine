/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: SDLWindow
Timestamp: 12/31/2018 2:05:56 AM

*/

#pragma once


#ifdef LLF_INPUTANDWINDOW_SDL

#ifndef SDLWindow_HPP
#define SDLWindow_HPP

#include "Lina/Rendering/Window.hpp"
#include "SDL.h"
#include "SDL_events.h"

namespace LinaEngine
{

	class Window_SDLGL : public Window
	{
	public:
		Window_SDLGL(const WindowProps& props);
		virtual ~Window_SDLGL();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		static Window* Create(const WindowProps& props = WindowProps());
		
		void SetMousePosition(const Vector2F& v) override;

	private:

		virtual void Init(const WindowProps& props);
		virtual void Shutdown();

	private:

		SDL_Window* m_Window;
		SDL_GLContext m_GLContext;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;

		static int WindowEventFilter(void* userdata, SDL_Event* event);
	};
}


#endif

#endif