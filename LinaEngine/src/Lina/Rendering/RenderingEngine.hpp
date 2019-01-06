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

#include "../Core.hpp"
#include "Window.hpp"


namespace LinaEngine
{
	class Event;
	class WindowResizeEvent;

	class LINA_API RenderingEngine
	{
	public:

		RenderingEngine();
		virtual ~RenderingEngine();

		virtual void OnUpdate();
		virtual void OnWindowEvent(Event& e) = 0;

		inline Window& GetMainWindow() const
		{ 
			LINA_CORE_ASSERT(m_Window, "Window pointer is null!");
			return *m_Window;
		}
		
	private:

		std::unique_ptr<Window> m_Window;
		

	};
}


#endif