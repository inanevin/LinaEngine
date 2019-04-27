/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: IMGUILayer
Timestamp: 3/7/2019 1:58:49 PM

*/

#pragma once

#ifndef Layer_IMGUI_HPP
#define Layer_IMGUI_HPP

#include "Core/Layer.hpp"
#include "Events/KeyEvent.hpp"
#include "Events/ApplicationEvent.hpp"
#include "Events/MouseEvent.hpp"

namespace LinaEngine
{
	class IMGUILayer : public Layer
	{

	public:

		LINA_API IMGUILayer();
		LINA_API ~IMGUILayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnEvent(Event& e) override;

	private:

		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);

	private:

		float m_Time = 0.0f;
	};
}


#endif