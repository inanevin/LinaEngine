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
10/9/2018 3:36:41 AM

*/

#pragma once

#ifndef Lina_EventSubscriber_H
#define Lina_EventSubscriber_H

#include "Lina_CoreMessageBus.h"
#include "Lina_Actions.h"
#include "Lina_SDLHandler.h"

#define CONDITION T
#define BINDING T*
#define CALLBACK std::function<void()>const&
#define CALLBACKPARAM std::function<void(T)>const&

class Lina_EventSubscriber
{
public:

	Lina_EventSubscriber();
	~Lina_EventSubscriber() {
		std::cout << "im dead" << std::endl;
	};
	/* ACITON SUBSCRIPTION OVERLOADS */
	void SubscribeToAction(ActionType, CALLBACK);
	template<typename T>
	void SubscribeToAction(ActionType, CALLBACKPARAM);
	template<typename T>
	void SubscribeToAction(ActionType, BINDING);
	template<typename T>
	void SubscribeToAction(ActionType, CONDITION, CALLBACK);
	template<typename T>
	void SubscribeToAction(ActionType, CONDITION, CALLBACKPARAM);
	template<typename T>
	void SubscribeToAction(ActionType, CONDITION, BINDING);
	template<typename T>
	void SubscribeToAction(ActionType, CALLBACK, CALLBACKPARAM);
	template<typename T>
	void SubscribeToAction(ActionType, CALLBACK, BINDING);
	template<typename T>
	void SubscribeToAction(ActionType, CALLBACKPARAM, BINDING);
	template<typename T>
	void SubscribeToAction(ActionType, CONDITION, CALLBACK, CALLBACKPARAM); 
	template<typename T>
	void SubscribeToAction(ActionType, CONDITION, CALLBACKPARAM, BINDING); 
	template<typename T>
	void SubscribeToAction(ActionType, CONDITION, CALLBACK, BINDING); 
	template<typename T>
	void SubscribeToAction(ActionType, CALLBACK, CALLBACKPARAM, BINDING); 
	template<typename T>
	void SubscribeToAction(ActionType, CONDITION, CALLBACK, CALLBACKPARAM, BINDING);

	std::list<std::shared_ptr<Lina_ActionHandlerBase>> m_Handlers;
};


#endif