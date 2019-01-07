/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: InputEngine
Timestamp: 1/6/2019 2:17:55 AM

*/

#pragma once
#ifndef InputEngine_HPP
#define InputEngine_HPP

#include "Lina/Events/Action.hpp"

namespace LinaEngine
{

	class Application;
	class InputEngine
	{

	public:

		InputEngine();
		virtual ~InputEngine();

		virtual void OnUpdate() = 0;
		virtual bool GetKey(int keyCode) = 0;
		virtual bool GetKeyDown(int keyCode) = 0;
		virtual bool GetKeyUp(int keyCode) = 0;
		virtual bool GetMouse(int keyCode) = 0;
		virtual bool GetMouseDown(int mouse) = 0;
		virtual bool GetMouseUp(int mouse) = 0;
		virtual Vector2F GetRawMouseAxis() = 0;
		virtual Vector2F GetMouseAxis() = 0;
		virtual Vector2F GetMousePosition() = 0;
		virtual void SetCursor(bool visible) const = 0;
		virtual void SetMousePosition(const Vector2F& v) const = 0;
		void SetApplication(Application& app);

		template<typename T>
		void SubscribeToAction(ActionType at, T condition, const std::function<void()>& cbp)
		{
			
			// Init the handler.
			ActionHandler<T> handler(at);
			handler.SetUseNoParamCallback(true);
			handler.SetUseCondition(true);
			handler.SetNoParamCallback(cb);
			handler.SetCondition(condition);

			// Push it as a shared ptr to the handler list.
			m_Handlers.push_back(std::make_shared<ActionHandler<T>>(handler));

			// Get the shared ptr pushed just now.
			std::list<std::shared_ptr<ActionHandlerBase>>::iterator iter = std::prev(m_Handlers.end());

			// construct a weakptr out of the shared.
			std::weak_ptr<ActionHandlerBase> wptr = *iter;

			
			// Call the subscription to subscribe to relevant dispatcher.
			Subscribe(at, wptr);
		}

		template<typename T>
		void SubscribeToAction(ActionType at, const std::function<void(T&)>& cbp)
		{
			// Init the handler.
			ActionHandler<T> handler(at);
			handler.SetUseParamCallback(true);
			handler.SetParamCallback(cbp);

			// Push it a shared ptr to the handler to the list.
			m_Handlers.push_back(std::make_shared<ActionHandler<T>>(handler));

			// Get the shared ptr pushed just now.
			std::list<std::shared_ptr<ActionHandlerBase>>::iterator iter = std::prev(m_Handlers.end());

			// construct a weakptr out of the shared.
			std::weak_ptr<ActionHandlerBase> wptr = *iter;

			// Call the subscription to subscribe to relevant dispatcher.
			Subscribe(at, wptr);
		}

		inline void Subscribe(ActionType at, std::weak_ptr<ActionHandlerBase> wptr)
		{
			if (at == ActionType::KeyPressed || at == ActionType::KeyReleased || at == ActionType::MouseButtonPressed || at == ActionType::MouseButtonReleased ||
				at == ActionType::MouseMotionX || at == ActionType::MouseMotionY || at == ActionType::SDLQuit)
			{
				m_InputDispatcher.SubscribeHandler(wptr);
			}
		}
		
	protected:

		std::list<std::shared_ptr<ActionHandlerBase>> m_Handlers;
		ActionDispatcher m_InputDispatcher;
		Application* app;
	};
}


#endif