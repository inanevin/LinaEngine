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

#include <list>
#include <type_traits>

namespace LinaEngine
{

	enum ActionType
	{
		KeyPressed,
		KeyReleased,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMotionX,
		MouseMotionY,
		SDLQuit
	};

	// Base wrapper class for actions.
	class ActionBase
	{

	public:
		ActionBase() {};
		ActionBase(ActionType t) : m_ActionType(t) {};
		virtual void* GetData() { return 0; }
		inline ActionType GetActionType() { return m_ActionType; }
		inline void SetActionType(ActionType t) { m_ActionType = t; }

	private:
		//Lina_ActionBase(const Lina_ActionBase& rhs) = delete;
		ActionType m_ActionType;
	};

	// Template class used for actions. 
	template<typename T = int>
	class Action : public ActionBase
	{

	public:
		Action() {};
		Action(ActionType t) : ActionBase::ActionBase(t) { }
		inline void SetData(T t) { m_Value = t; }
		virtual void* GetData() { return &m_Value; }

	private:
		T m_Value;

	};

	// Base wrapper class for action handlers.
	class ActionHandlerBase
	{
	public:

		ActionHandlerBase() {}
		~ActionHandlerBase() {};
		ActionHandlerBase(ActionType at) : m_ActionType(at) { };

		inline ActionType GetActionType() { return m_ActionType; }
		inline void SetActionType(ActionType t) { m_ActionType = t; }

	protected:
		friend class ActionDispatcher;

		virtual void Control(ActionBase& action) { };
		virtual void Execute(ActionBase& action) {};


	private:
		ActionType m_ActionType;
	};

	// Service class for actions handlers, determines the behaviour choices of actions.
	template<typename T>
	class ActionHandler_ConditionCheck : public ActionHandlerBase
	{

	public:

		ActionHandler_ConditionCheck() {};
		~ActionHandler_ConditionCheck() {};
		ActionHandler_ConditionCheck(ActionType at) :
			ActionHandlerBase::ActionHandlerBase(at) { };

		inline void SetCondition(T t) { m_Condition = t; }
		inline T GetCondition() { return m_Condition; }

	protected:

		friend class ActionDispatcher;

		// Control block called by the dispatchers.
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

		template<typename U>
		bool CompareValue(U u)
		{
			return LinaEngine::Internal::comparison_traits<T>::equal(m_Condition, u);
		}


	private:
		T m_Condition;

	};



	// Main derived class used for action handlers.
	template<typename T = int>
	class ActionHandler : public ActionHandler_ConditionCheck<T>
	{
	public:

		ActionHandler() {}
		~ActionHandler() {}
		ActionHandler(ActionType at) :
			ActionHandler_ConditionCheck<T>::ActionHandler_ConditionCheck(at) {
		};

		inline void SetUseCondition(bool b) { m_UseCondition = b; }
		inline void SetUseBinding(bool b) { m_UseBinding = b; }
		inline void SetUseParamCallback(bool b) { m_UseParamCallback = b; }
		inline void SetUseNoParamCallback(bool b) { m_UseNoParamCallback = b; }


		inline bool GetUseBinding() { return m_UseBinding; }
		inline bool GetConditionCheck() { return m_UseCondition; }
		inline bool GetUseParamCallback() { return m_UseParamCallback; }
		inline bool GetUseNoParamCallback() { return m_UseNoParamCallback; }

		inline void SetParamCallback(const std::function<void(T&)>& cbp) { m_CallbackParam = cbp; }
		inline void SetNoParamCallback(const std::function<void()>& cb) { m_CallbackNoParam = cb; }
		inline void SetBinding(T* binding) { m_Binding = binding; }


	protected:

		friend class ActionDispatcher;

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
			// Call the callback with no parameters
			if (m_UseNoParamCallback)
				m_CallbackNoParam();

			// If we use parameterized callback or binding, we will extract the value from the action.
			// However, if we have not used condition, it means whe have not typed checked this value. So type check it first.
			if ((m_UseParamCallback || m_UseBinding))
			{
				// Cast from polymorphic action base class void* type to T*.
				T* typePointer = static_cast<T*>(action.GetData());

				if (!m_UseCondition)
				{
					// If the types do not match, simply exit.
					if (!ActionHandler_ConditionCheck<T>::CompareType(*typePointer))
						return;
				}

				// Bind the value.
				if (m_UseBinding)
					*m_Binding = *typePointer;

				// Call the callback with parameters, cast and pass in the data from the action.
				if (m_UseParamCallback)
					m_CallbackParam(*typePointer);
			}

		}

	private:

		bool m_UseParamCallback = false;
		bool m_UseNoParamCallback = false;
		bool m_UseBinding = false;
		bool m_UseCondition = false;
		T* m_Binding;
		std::function<void()> m_CallbackNoParam;
		std::function<void(T&)> m_CallbackParam;
	};

	class TestClass;

	// Dispatcher class for actions.
	class ActionDispatcher
	{
		typename std::list<std::weak_ptr<ActionHandlerBase>>::iterator it;

	public:

		ActionDispatcher() { };

		~ActionDispatcher()
		{
			m_ActionHandlers.clear();
		}

		void operator=(ActionDispatcher const&) = delete;

		void DispatchAction(ActionBase& action)
		{

			for (it = m_ActionHandlers.begin(); it != m_ActionHandlers.end(); it++)
			{
				// Check if the the object is alive.
				if (auto tmp = it->lock())
				{
					// Check if the action types match.
					if (action.GetActionType() == tmp->GetActionType())
					{
						// Tell the handler to check the action.
						tmp->Control(action);
					}
				}
			}

			// Check the handlers if any of the pointed objects are dead, remove it from the list if so.
			m_ActionHandlers.erase(std::remove_if(m_ActionHandlers.begin(), m_ActionHandlers.end(),
				[](std::weak_ptr<ActionHandlerBase> handler)
			{
				return handler.expired();	// LOCK OR EXPIRED?
			}), m_ActionHandlers.end());


		}

		void SubscribeHandler(std::weak_ptr<ActionHandlerBase> ptr)
		{
			// Add the weak pointer to the list.
			m_ActionHandlers.push_back(ptr);
		}

	private:

		std::list<std::weak_ptr<ActionHandlerBase>> m_ActionHandlers;

	};
}


#endif