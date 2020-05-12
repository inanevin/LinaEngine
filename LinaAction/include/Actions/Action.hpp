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
	enum  ActionType
	{
		KeyPressed,
		KeyReleased,
		MouseButtonPressed,
		MouseButtonReleased,
		ACTION_TYPES_LASTINDEX = MouseButtonReleased
	};


	class ActionHandlerParamsBase
	{

	public:

		virtual ~ActionHandlerParamsBase () {};

	protected:

		ActionHandlerParamsBase(ActionType type) : m_ActionType(type), m_UseCondition(false) {};

		bool m_UseCondition = false;
		ActionType m_ActionType;


	private:

	};

	template<typename T>
	class ActionHandlerParams : public ActionHandlerParamsBase
	{
	public:

		virtual ~ActionHandlerParams() {};

	private:

		friend class ActionHandlerFactory;

		ActionHandlerParams(ActionType type)
			: ActionHandlerParamsBase(type){};


		ActionType actionType;
		T condition;
		std::function<void(T)> callback;

	private:

		bool useCondition = false;

	};


	class ActionHandlerBase
	{

	public:

		DISALLOW_COPY_AND_ASSIGN(ActionHandlerBase);

		virtual ~ActionHandlerBase() {};
		FORCEINLINE ActionType GetActionType() const { return m_ActionType; }
		FORCEINLINE void* GetCondition() const { return 0; }
		FORCEINLINE bool GetUseCondition() const { return useCondition; }

	protected:

		ActionHandlerBase(ActionType at) : m_ActionType(at) {};

		bool useCondition = false;
		ActionType m_ActionType;

	};

	template<typename T>
	class ActionHandler : public ActionHandlerBase
	{

	public:

		DISALLOW_COPY_AND_ASSIGN(ActionHandler);

		
		virtual ~ActionHandler() {};

		FORCEINLINE void SetCondition(const T& cond) { m_Condition = cond; useCondition = true; }
		FORCEINLINE void SetCallback(const std::function<void(T)>& cb) { m_Callback = cb; }
		FORCEINLINE void* GetCondition() const { return &m_Condition; }
		FORCEINLINE void ControlExecute(const T& data) { if (!useCondition || ( useCondition && m_Condition == data)) m_Callback(data); }


	private:

		friend class ActionSubscriber;
		ActionHandler(ActionType at) : ActionHandlerBase(at) {};

		std::function<void(T)> m_Callback;
		T m_Condition;
	};


	
}


#endif