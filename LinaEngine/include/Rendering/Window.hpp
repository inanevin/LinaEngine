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


#include "PackageManager/PAMWindow.hpp"
#include "Utility/Log.hpp"

namespace LinaEngine
{
	namespace Input
	{
		class InputEngine;
	}
}

namespace LinaEngine::Graphics
{
	class Window
	{
	public:

		FORCEINLINE Window()
		{
			LINA_CORE_TRACE("[Constructor] -> Window ({0})", typeid(*this).name());
		}

		virtual ~Window()
		{
			LINA_CORE_TRACE("[Destructor] -> Window ({0})", typeid(*this).name());
		};

		// Initializes the window.
		FORCEINLINE bool Initialize(LinaEngine::Input::InputEngine& inputEngineIn) { return m_Derived.Initialize(inputEngineIn, m_Properties); }

		// Enables/disables vsync.
		FORCEINLINE void SetVsync(bool enabled) { m_Properties.vSyncEnabled = enabled; m_Derived.SetVsync(enabled); }

		// Sets event callback reference.
		FORCEINLINE void SetEventCallback(const std::function<void(Event&)>& callback) { m_Derived.SetEventCallback(callback); }

		// Get vsync state.
		FORCEINLINE bool GetVsycnEnabled() { return m_Properties.vSyncEnabled; }

		// Get window width.
		FORCEINLINE float GetWidth() { return (float)m_Properties.m_Width; }

		// Get window height.
		FORCEINLINE float GetHeight() { return (float)m_Properties.m_Height; }

		// Get pointer to native window.
		FORCEINLINE void* GetNativeWindow() const { return m_Derived.GetNativeWindow(); }

		// Called every frame.
		FORCEINLINE void Tick() { m_Derived.Tick(); }

	private:

		WindowProperties m_Properties;
		ContextWindow m_Derived;
	};
}

#endif