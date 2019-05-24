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

#include "Events/Event.hpp"


namespace LinaEngine
{
	/* Struct containing basic data about window properties. */
	struct WindowProperties
	{
		std::string m_Title;
		unsigned int m_Width;
		unsigned int m_Height;
		bool vSyncEnabled;

		WindowProperties()
		{
			m_Title = "Lina Engine";
			m_Width = 1440;
			m_Height = 900;
		}

		WindowProperties(const std::string& title, unsigned int width, unsigned int height)
		{
			m_Title = title;
			m_Width = width;
			m_Height = height;
		}
	};

	template<class Derived>
	class Window
	{
	public:

		virtual ~Window()
		{
			LINA_CORE_TRACE("[Destructor] -> Window ({0})", typeid(*this).name());
		};

		FORCEINLINE bool Initialize() { return m_Derived->Initialize_Impl(); }
		FORCEINLINE void SetVsync(bool enabled) { m_Properties.vSyncEnabled = enabled; m_Derived->SetVsync_Impl(enabled); }
		FORCEINLINE void SetEventCallback(const std::function<void(Event&)>& callback) { m_EventCallback = callback; }
		FORCEINLINE bool GetVsycnEnabled() { return m_Properties.vSyncEnabled; }
		FORCEINLINE float GetWidth() { return (float)m_Properties.m_Width; }
		FORCEINLINE float GetHeight() { return (float)m_Properties.m_Height; }
		FORCEINLINE void* GetNativeWindow() const { return m_Derived->GetNativeWindow_Impl(); }
		FORCEINLINE void Tick() { m_Derived->Tick_Impl(); }

	protected:

		Window(const WindowProperties& props) : m_Properties(props) 
		{ 
			LINA_CORE_TRACE("[Constructor] -> Window ({0})", typeid(*this).name());
			m_Derived = static_cast<Derived*>(this);
		};

		WindowProperties m_Properties;
		std::function<void(Event&)> m_EventCallback;

	private:

		Derived* m_Derived;
	};
}

#endif