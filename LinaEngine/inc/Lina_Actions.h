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
#include <list>
#include "pch.h"
#include <variant>
#include <type_traits>

enum ActionType
{
	ActionType1,
	ActionType2
};

enum ActionBindType
{
	BindValue,
	BindMethod,
	BindMethodToValue
};




class Lina_ActionBase
{
public:
	Lina_ActionBase() {};
	virtual void* GetData() { return 0; }
	ActionType GetActionType() { return m_ActionType; }
private:
	Lina_ActionBase(const Lina_ActionBase& rhs) = delete;
	ActionType m_ActionType;
};

template<typename T>
class Lina_Action : public Lina_ActionBase
{
public:

	Lina_Action() { std::cout << "CONST" << std::endl; }
	std::string id;
	T m_Data;
	void SetData(T t) { m_Data = t; }
	virtual void* GetData() {return &m_Data; }
};


class Lina_ActionHandlerBase
{
public:

	Lina_ActionHandlerBase(){}
	Lina_ActionHandlerBase(ActionType at, ActionBindType abt) : m_ActionType(at), m_ActionBindType(abt)
	{
		// Constructor.
	};
	ActionType GetActionType() { return m_ActionType; }
	ActionBindType GetActionBindType() { return m_ActionBindType; }

	virtual void Control(Lina_ActionBase& action) { };
	virtual void Execute(Lina_ActionBase& action) {};


private:


	ActionType m_ActionType;
	ActionBindType m_ActionBindType;
};


/*class Lina_ActionHandlerMethod : public Lina_ActionHandlerBase
{

public:

	Lina_ActionHandlerMethod(ActionType at, std::function<void()>&& cb)
	{
		m_ActionType = at;
		m_ActionBindType = NoControl;
		m_Callback = cb;
	}

protected:

	virtual void Execute() override
	{
		m_Callback();
	}

private:

	std::function<void()> m_Callback;
};*/


template<typename T>
class Lina_ActionHandler_ValCheck : public Lina_ActionHandlerBase
{

public:


	Lina_ActionHandler_ValCheck() {};

	Lina_ActionHandler_ValCheck(ActionType at, ActionBindType abt, T condition) :
	Lina_ActionHandlerBase::Lina_ActionHandlerBase(at, abt), m_Value(condition)
	{
		// Constructor.
	}

	virtual void Control(Lina_ActionBase& action) override
	{
		T* typePointer = static_cast<T*>(action.GetData()); // cast from void* to int*
		std::cout << typePointer << std::endl;
		std::cout << action.GetData() << std::endl;
		if (CompareData(*typePointer))
		{
			Execute(action);
		}
	}

	void SetMyValue(T t) { m_Value = t; }
	T GetValue() { return m_Value; }

	template<typename U>
	bool CompareData(U u)
	{
		if (std::is_same<U, T>::value)
			return LinaEngine::Internal::comparison_traits<T>::equal(m_Value, u);
			
		return false;
	}
	

protected:
	T m_Value;
};


template<typename T>
class Lina_ActionHandler_CallMethod : public Lina_ActionHandler_ValCheck<T>
{
public:

	Lina_ActionHandler_CallMethod() {}

	Lina_ActionHandler_CallMethod(ActionType at, ActionBindType abt, T condition):
	Lina_ActionHandler_ValCheck<T>::Lina_ActionHandler_ValCheck(at, abt, condition)
	{
		// Constructor.
	}

	virtual void Control(Lina_ActionBase& action) override
	{
		std::cout << "3rd control" << std::endl;
		Lina_ActionHandler_ValCheck<T>::Control(action);
		//std::cout << "subsub control";
	}

	virtual void Execute(Lina_ActionBase& action) override
	{
		
	}


};



class Lina_ActionDispatcher
{

public:

	Lina_ActionDispatcher()
	{

	}
	std::list<Lina_ActionHandlerBase*> m_TestListeners;
	std::list<std::weak_ptr<Lina_ActionHandlerBase>> m_ActionHandlers;

};


class TestClass
{
public:
	TestClass()
	{

		Lina_ActionDispatcher* disp = new Lina_ActionDispatcher;
		Lina_Action<float> action;
		action.SetData(23);
		action.id = "xd";
		float myBindableData = 23;

		Lina_ActionHandler_CallMethod<float> b(ActionType::ActionType1, ActionBindType::BindValue, myBindableData);


		disp->m_TestListeners.push_back(&b);

		disp->m_TestListeners.front()->Control(action);
		

	}

};
//static bool deleteAll(Foo * theElement) { delete theElement; return true; }

//foo_list.remove_if(deleteAll);

#endif