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

#include "LinaPch.hpp"


namespace LinaEngine
{
	enum ActionType
	{
		KeyPressed,
		KeyReleased,
		MouseButtonPressed,
		MouseButtonReleased,
		ACTION_TYPES_LASTINDEX = MouseButtonReleased
	};


	class ActionHandlerBase
	{

	public:

		DISALLOW_COPY_AND_ASSIGN(ActionHandlerBase);

		virtual ~ActionHandlerBase() {};
		FORCEINLINE ActionType GetActionType() const { return m_ActionType; }

	protected:

		ActionHandlerBase(ActionType at) : m_ActionType(at) {};

	private:

		ActionType m_ActionType;
	};

	template<typename T>
	class ActionHandler : ActionHandlerBase
	{

	public:

		DISALLOW_COPY_AND_ASSIGN(ActionHandler);


		FORCEINLINE void SetCondition(const T& cond) { m_Condition = cond; useCondition = true; }
		FORCEINLINE void SetCallback(const std::function<void(T&)>& cb) { m_Callback = cb; }
		
		// Compares equality of the value of type T w/ the value of type U 
		template<typename U>
		static bool CompareValue(U u)
		{
			return LinaEngine::Internal::comparison_traits<T>::equal(m_Condition, u);
		}

	private:

		template<typename T> friend class Action;
		ActionHandler(ActionType at) : m_ActionType(at) {};

	private:

		bool useCondition;
		std::function<void(T&)> m_Callback;
		ActionType m_ActionType;
		T m_Condition;
	};


	class ActionBase
	{

	public:

		DISALLOW_COPY_AND_ASSIGN(ActionBase);

		virtual ~ActionBase() {};

		FORCEINLINE virtual void* GetData() const { return 0; }
		FORCEINLINE ActionType GetActionType() const { return m_ActionType; }

	protected:

		ActionBase(ActionType at) : m_ActionType(at) {};

	private:

		ActionType m_ActionType;
		void* m_Data;
	};


	template<typename T>
	class Action : public ActionBase
	{

	public:

		DISALLOW_COPY_AND_ASSIGN(Action);

		Action(ActionType at) : ActionBase(at) {};
		virtual ~Action() {};

		FORCEINLINE T& GetData() const { return m_Data; }

		FORCEINLINE static ActionHandlerBase* CreateHandler()
		{
			return new ActionHandler<T>(m_ActionType);
		}

		// Sets the data of this action
		FORCEINLINE void SetData(const T& data) { m_Data = t; }
		FORCEINLINE void* GetData() override { return &m_Data; }

	private:

		T m_Data;
	};
}


#endif