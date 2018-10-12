/*
Author: Inan Evin
www.inanevin.com

MIT License

Lina Engine, Copyright (c) 2018 Inan Evin

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

4.0.30319.42000
10/9/2018 3:36:41 AM

*/

#pragma once

#ifndef Lina_EventSubscriber_H
#define Lina_EventSubscriber_H

#include "Lina_CoreMessageBus.h"
#include "Lina_Actions.h"
#include "Lina_InputEngine.h"

#define CONDITION T
#define BINDING T*
#define ACTIONCALLBACK std::function<void()>const&
#define ACTIONCALLBACKPARAM std::function<void(T)>const&

class Lina_EventHandler
{
public:

	Lina_EventHandler() {};
	~Lina_EventHandler() {};
	void Initialize();

	/* ACITON SUBSCRIPTION OVERLOADS */
	void SubscribeToAction(ActionType, ACTIONCALLBACK);
	template<typename T>
	void SubscribeToAction(ActionType, ACTIONCALLBACKPARAM);
	template<typename T>
	void SubscribeToAction(ActionType, BINDING);
	template<typename T>
	void SubscribeToAction(ActionType, CONDITION, ACTIONCALLBACK);
	template<typename T>
	void SubscribeToAction(ActionType, CONDITION, ACTIONCALLBACKPARAM);
	template<typename T>
	void SubscribeToAction(ActionType, CONDITION, BINDING);
	template<typename T>
	void SubscribeToAction(ActionType, ACTIONCALLBACK, ACTIONCALLBACKPARAM);
	template<typename T>
	void SubscribeToAction(ActionType, ACTIONCALLBACK, BINDING);
	template<typename T>
	void SubscribeToAction(ActionType, ACTIONCALLBACKPARAM, BINDING);
	template<typename T>
	void SubscribeToAction(ActionType, CONDITION, ACTIONCALLBACK, ACTIONCALLBACKPARAM); 
	template<typename T>
	void SubscribeToAction(ActionType, CONDITION, ACTIONCALLBACKPARAM, BINDING); 
	template<typename T>
	void SubscribeToAction(ActionType, CONDITION, ACTIONCALLBACK, BINDING); 
	template<typename T>
	void SubscribeToAction(ActionType, ACTIONCALLBACK, ACTIONCALLBACKPARAM, BINDING); 
	template<typename T>
	void SubscribeToAction(ActionType, CONDITION, ACTIONCALLBACK, ACTIONCALLBACKPARAM, BINDING);
	void Subscribe(ActionType at, std::weak_ptr<Lina_ActionHandlerBase>);

	float GetMouseX();
	float GetMouseY();
	float GetRawMouseX();
	float GetRawMouseY();

	std::list<std::shared_ptr<Lina_ActionHandlerBase>> m_Handlers;

	// Instance references.
	Lina_InputEngine* inputEngine;
	Lina_ActionDispatcher* inputDispatcher;
};


#endif