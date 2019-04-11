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

	// Base wrapper class for actions.
	class ActionBase
	{
	public:
		/* Gets the data as user pointer. Logically this is pure, but needed to implement due to dynamic type casting. */
		virtual void* GetData() { return 0; }

		/* Get the action type. */
		FORCEINLINE ActionType GetActionType() { return m_ActionType; }

	protected:
		/* Protected param constructor */
		ActionBase(ActionType t) : m_ActionType(t) {};

	private:
		ActionType m_ActionType;
	};

	/* Template class used for actions. */
	template<typename T = int>
	class Action : public ActionBase
	{
	public:
		/* Param constructor */
		Action(ActionType t) : ActionBase::ActionBase(t) { }

		/* Sets the action data, uses template type. */
		FORCEINLINE void SetData(const T& t) { m_Data = t; }

		/* Overrides the get data from the base class, this is the actual implemented version. */
		FORCEINLINE virtual void* GetData() override { return &m_Data; }

	private:
		T m_Data;
	};

	// Base wrapper class for action handlers.
	class ActionHandlerBase
	{
	public:

		/* Virtual destructor */
		virtual ~ActionHandlerBase() {};

		/* Gets the action type. */
		FORCEINLINE ActionType GetActionType() { return m_ActionType; }

		/* Gets the caller address. */
		FORCEINLINE void* GetCaller() { return m_Caller; }

		/* Control is implemented at the condition check subclass. */
		virtual void Control(ActionBase& action) = 0;

	protected:
		/* Virtual param constructor.*/
		ActionHandlerBase(ActionType at, void* caller) : m_ActionType(at), m_Caller(caller) { };

		/* Execute is implemented at the lower handler subclass. */
		virtual void Execute(ActionBase& action) = 0;

	protected:
		void* m_Caller;
	
	private:
		ActionType m_ActionType;
	};

	// Service class for actions handlers, determines the behaviour choices of actions.
	template<typename T>
	class ActionHandler_ConditionCheck : public ActionHandlerBase
	{
	public:

		/* Virtual destructor */
		virtual ~ActionHandler_ConditionCheck() {};

		/* Mutators for condition & callback set. */
		FORCEINLINE void SetCondition(const T& t) { m_Condition = t;  m_UseCondition = true; }
		FORCEINLINE void SetCallback(const std::function<void(T&)>& cb) { m_Callback = cb;  }

	protected:

		/* Param constructor. */
		ActionHandler_ConditionCheck(ActionType at, void* caller) :
			ActionHandlerBase::ActionHandlerBase(at, caller) { };

		/* Control block called by the dispatchers. */
		virtual void Control(ActionBase& action) override
		{
			// Cast from polymorphic action base class void* type to T*.
			T* typePointer = static_cast<T*>(action.GetData());

			// Compare the type of the member attribute of type of the value value received from the action.
			// If types are the same, then check the values, if they are the same, execute
			if (CompareType(*typePointer) && CompareValue(*typePointer))
				Execute(action);
		}

		// Compares the data type U with member attribute of type T. First chekcs if T & U are the same time, then if they are equal.
		template<typename U>
		bool CompareType(U u)
		{
			if (std::is_same<U, T>::value)
				return true;

			return false;
		}

		/* Compares equality of the value of type T w/ the value of type U */
		template<typename U>
		bool CompareValue(U u)
		{
			return LinaEngine::Internal::comparison_traits<T>::equal(m_Condition, u);
		}

	protected:
		bool m_UseCondition = false;
		T m_Condition;
		std::function<void(T&)> m_Callback;
	};

	// Main derived class used for action handlers.
	template<typename T = int>
	class ActionHandler : public ActionHandler_ConditionCheck<T>
	{
	public:

		/* Virtual destructor */
		virtual ~ActionHandler() {}

		/* Param constructor */
		ActionHandler(ActionType at, void* caller) :
			ActionHandler_ConditionCheck<T>::ActionHandler_ConditionCheck(at, caller) {
		};
	protected:

		virtual void Control(ActionBase& action) override
		{
			// If condition check is used, call the control of the behaviour base class so it can compare it's member attribute T with the action's value.
			if (m_UseCondition)
				ActionHandler_ConditionCheck<T>::Control(action);
			else
				Execute(action);	// Execute the action if no check is provided.
		}

		virtual void Execute(ActionBase& action) override
		{
			// Cast from polymorphic action base class void* type to T*.
			T* typePointer = static_cast<T*>(action.GetData());

			if (!m_UseCondition)
			{
				// If the types do not match, simply exit.
				if (!ActionHandler_ConditionCheck<T>::CompareType(*typePointer))
					return;
			}

			// Call the callback with parameters, cast and pass in the data from the action.
			m_Callback(*typePointer);
		}
	};
}


#endif