/*
Author: Inan Evin
www.inanevin.com

BSD 2-Clause License
Lina Engine Copyright (c) 2018, Inan Evin All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.

-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO
-- THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
-- BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
-- GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
-- STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
-- OF SUCH DAMAGE.

4.0.30319.42000
10/6/2018 11:26:54 PM

*/

#pragma once

#ifndef Lina_Actions_H
#define Lina_Actions_H
#include "pch.h"
#include <list>
#include <type_traits>

enum ActionType
{
	ActionType1,
	ActionType2
};

// Base wrapper class for actions.
class Lina_ActionBase
{

public:

	Lina_ActionBase() {};
	virtual void* GetData() { return 0; }
	inline ActionType GetActionType() { return m_ActionType; }

private:
	Lina_ActionBase(const Lina_ActionBase& rhs) = delete;
	ActionType m_ActionType;

};

// Template class used for actions. 
template<typename T>
class Lina_Action : public Lina_ActionBase
{

public:

	Lina_Action() { }
	inline void SetData(T t) { m_Value = t; }
	virtual void* GetData() { return &m_Value; }

private:
	T m_Value;

};

// Base wrapper class for action handlers.
class Lina_ActionHandlerBase
{
public:

	Lina_ActionHandlerBase() {}
	~Lina_ActionHandlerBase() {};
	Lina_ActionHandlerBase(ActionType at) : m_ActionType(at) { };

	inline ActionType GetActionType() { return m_ActionType; }
	inline void SetActionType(ActionType t) { m_ActionType = t; }
	virtual void Control(Lina_ActionBase& action) { };
	virtual void Execute(Lina_ActionBase& action) {};


private:
	ActionType m_ActionType;
};

// Service class for actions handlers, determines the behaviour choices of actions.
template<typename T>
class Lina_ActionHandler_ConditionCheck : public Lina_ActionHandlerBase
{

public:

	Lina_ActionHandler_ConditionCheck() {};
	~Lina_ActionHandler_ConditionCheck() {};
	Lina_ActionHandler_ConditionCheck(ActionType at) :
		Lina_ActionHandlerBase::Lina_ActionHandlerBase(at) { };


	inline void SetCondition(T t) { m_Condition = t; }
	inline T GetCondition() { return m_Condition; }

	// Control block called by the dispatchers.
	virtual void Control(Lina_ActionBase& action) override
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
class Lina_ActionHandler : public Lina_ActionHandler_ConditionCheck<T>
{
public:

	Lina_ActionHandler() {}
	~Lina_ActionHandler() {}
	Lina_ActionHandler(ActionType at) :
		Lina_ActionHandler_ConditionCheck<T>::Lina_ActionHandler_ConditionCheck(at) {};

	inline void SetUseCondition(bool b) { m_UseCondition = b; }
	inline void SetUseBinding(bool b) { m_UseBinding = b; }
	inline void SetUseParamCallback(bool b) { m_UseParamCallback = b; }
	inline void SetUseNoParamCallback(bool b) { m_UseNoParamCallback = b; }


	inline bool GetUseBinding() { return m_UseBinding; }
	inline bool GetConditionCheck() { return m_UseCondition; }
	inline bool GetUseParamCallback() { return m_UseParamCallback; }
	inline bool GetUseNoParamCallback() { return m_UseNoParamCallback; }

	virtual void Control(Lina_ActionBase& action) override
	{

		// If condition check is used, call the control of the behaviour base class so it can compare it's member attribute T with the action's value.
		if (m_UseCondition)
			Lina_ActionHandler_ConditionCheck<T>::Control(action);
		else
			Execute(action);	// Execute the action if no check is provided.
	}

	virtual void Execute(Lina_ActionBase& action) override
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
				if (Lina_ActionHandler_ConditionCheck<T>::CompareType(*typePointer))
					return;
			}
			
		
			// Call the callback with parameters, cast and pass in the data from the action.
			if (m_UseParamCallback)
				m_CallbackParam(*typePointer);

			// Bind the value.
			if (m_UseBinding)
				*m_Binding = *typePointer;
		}
	
	}

	void SetParamCallback(std::function<void(T)> && cbp) { m_CallbackParam = cbp; }
	void SetNoParamCallback(std::function<void()>&& cb) { m_CallbackNoParam = cb; }
	void SetBinding(T* binding) { m_Binding = binding; }

private:
	bool m_UseParamCallback = false;
	bool m_UseNoParamCallback = false;
	bool m_UseBinding = false;
	bool m_UseCondition = false;
	T* m_Binding;
	std::function<void()> m_CallbackNoParam;
	std::function<void(T)> m_CallbackParam;
};


// Dispatcher class for actions.
class Lina_ActionDispatcher
{

public:

	Lina_ActionDispatcher(){}
	~Lina_ActionDispatcher(){}

	void SubscribeHandler()
	{

	}

	std::list<Lina_ActionHandlerBase*> m_TestListeners;
	std::list<std::weak_ptr<Lina_ActionHandlerBase>> m_ActionHandlers;

};


using namespace std;
class TestClass
{
public:
	TestClass()
	{
		int toBind = 2;

		Lina_ActionDispatcher* disp = new Lina_ActionDispatcher;
		Lina_Action<float> action;
		
		action.SetData(22.5);

		auto f = []() {std::cout << "Non-Parameterized Callback Called" << std::endl; };
		auto f2 = [](float f) {std::cout << "Parameterized Callback Called, Value is : " << f << std::endl; };

		Lina_ActionHandler<float> b(ActionType1);

		b.SetUseCondition(true);
		b.SetUseNoParamCallback(true);
		b.SetUseParamCallback(true);

		b.SetCondition(22.5);
		b.SetNoParamCallback(f);
		b.SetParamCallback(f2);

		disp->m_TestListeners.push_back(&b);
		disp->m_TestListeners.front()->Control(action);

		std::cout << "Binded Variable Is: " << toBind;
	}

};

//static bool deleteAll(Action * theElement) { delete action; return true; }
//actList.remove_if(deleteAll);

#endif