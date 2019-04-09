/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: RenderingEngine
Timestamp: 1/2/2019 10:51:47 PM

*/

#pragma once
#ifndef RenderingEngine_HPP
#define RenderingEngine_HPP

#include "Lina/Core/Core.hpp"
#include "Window.hpp"
#include "Lina/Rendering/Texture.hpp"
#include "Lina/Camera.hpp"

namespace LinaEngine
{
	class WindowResizeEvent;


	class LINA_API RenderingEngine
	{
	public:

		RenderingEngine();
		virtual ~RenderingEngine();

		/* Initializes the rendering engine. */
		virtual void Initialize();

		/* Starts the rendering engine. */
		virtual void Start() = 0;

		/* Called in each frame. */
		virtual void OnUpdate();

		/* Called when an event occurs */
		virtual void OnEvent(class Event& e) = 0;

		/* Called when a window event occurs. */
		virtual void OnWindowEvent(class Event& e) = 0;

		/* Sets the application reference. */
		virtual void SetApplication(class Application&) = 0;

		/* Sets the mouse position to desired screen space coordinates. */
		virtual void SetMousePosition(const Vector2F& v) = 0;

		/* Enables & Disables wireframe mode.*/
		virtual void SetWireframeMode(bool activation) = 0;

		/* return the main window reference. */
		inline Window& GetMainWindow() const { LINA_CORE_ASSERT(m_Window, "Window is nullptr!"); return *m_Window; }

		inline bool GetIsWireframeModeActive() { return isWireframeModeActive; }

	protected:

		bool isWireframeModeActive;
		WindowProps m_WindowProps;

	private:

		/* Window that the rendering engine is rendering within. */
		std::unique_ptr<Window> m_Window;


	};
}


#endif