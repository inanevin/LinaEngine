/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Action
Timestamp: 1/6/2019 5:41:20 AM

*/

#pragma once

#ifndef Action_HPP
#define Action_HPP

#include "Core/Common.hpp"
#include <functional>

namespace LinaEngine::Action
{
	enum ActionType
	{
		KeyPressed = 0,
		KeyReleased,
		MouseButtonPressed,
		MouseButtonReleased,
		MessageLogged,
		ActionTypesLastIndex = MessageLogged
	};

	class ActionHandlerBase
	{

	public:

		virtual ~ActionHandlerBase() {};
		FORCEINLINE ActionType GetActionType() const { return m_actionType; }
		FORCEINLINE bool GetUseCondition() const { return m_useCondition; }

	protected:

		ActionHandlerBase(ActionType at) : m_actionType(at) {};

	protected:

		bool m_useCondition = false;
		ActionType m_actionType;

	};

	template<typename T>
	class ActionHandler : public ActionHandlerBase
	{

	public:
	
		virtual ~ActionHandler() {};

		FORCEINLINE void SetCondition(const T& cond) { m_condition = cond; m_useCondition = true; }
		FORCEINLINE void SetCallback(const std::function<void(T)>& cb) { m_callback = cb; }

		// Checks condition if one is used and invokes the callback.
		FORCEINLINE void ControlExecute(const T& data) 
		{ 
			if (!m_useCondition || ( m_useCondition && m_condition == data))
				m_callback(data);
		}

	private:

		friend class ActionSubscriber;
		ActionHandler(ActionType at) : ActionHandlerBase(at) {};

	private:

		std::function<void(T)> m_callback;
		T m_condition;
	};


	
}


#endif