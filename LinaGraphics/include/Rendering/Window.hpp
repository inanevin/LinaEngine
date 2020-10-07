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
Timestamp: 4/14/2019 7:46:12 PM

*/

#pragma once

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <functional>
#include "Utility/Math/Vector.hpp"
#include "RenderingCommon.hpp"

namespace LinaEngine::Graphics
{
	class Window
	{
	public:

		Window() {};

		virtual ~Window() {};

		// Initializes the window.
		virtual bool CreateContext(WindowProperties& propsIn) = 0;

		// Called every frame.
		virtual void Tick() = 0;

		// Get pointer to native window.
		virtual void* GetNativeWindow() const = 0;

		// Enables/disables vsync.
		virtual void SetVsync(bool enabled)
		{
			m_windowProperties.vSyncEnabled = enabled;
		}

		// returns window active time.
		virtual double GetTime() = 0;

		// Resizes the native window.
		virtual void Resize(const Vector2& newSize) = 0;

		// Get vsync state.
		FORCEINLINE bool GetVsycnEnabled() { return m_windowProperties.vSyncEnabled; }

		// Get window width.
		FORCEINLINE uint32 GetWidth() { return m_windowProperties.m_Width; }

		// Get window height.
		FORCEINLINE uint32 GetHeight() { return m_windowProperties.m_Height; }

		// Gets size
		FORCEINLINE Vector2 GetSize() { return Vector2(m_windowProperties.m_Width, m_windowProperties.m_Height); }

		// Set event callbacks.
		FORCEINLINE void SetKeyCallback(std::function<void(int, int)>& callback) { m_keyCallback = callback; }
		FORCEINLINE void SetMouseCallback(std::function<void(int, int)>& callback) { m_mouseCallback = callback; }
		FORCEINLINE void SetWindowResizeCallback(std::function<void(Vector2)>& callback) { m_windowResizeCallback = callback; }
		FORCEINLINE void SetWindowClosedCallback(std::function<void()>& callback) { m_windowCloseCallback = callback; }
	
	protected:

		std::function<void(int, int)> m_keyCallback;
		std::function<void(int, int)> m_mouseCallback;
		std::function<void(Vector2)> m_windowResizeCallback;
		std::function<void()> m_windowCloseCallback;
		WindowProperties m_windowProperties;
	};
}

#endif