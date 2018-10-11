/*
Author: Inan Evin
www.inanevin.com

BSD 2-Clause License
Lina Engine Copyright (c) 2018, Inan Evin All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following T conds are met:

* Redistributions of source code must retain the above copyright notice, this list of T conds and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of T conds and the following disclaimer in the documentation
* and/or other materials provided with the distribution.

-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO
-- THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
-- BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
-- GOODS OR SERVICES { } LOSS OF USE, DATA, OR PROFITS { } OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
-- STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
-- OF SUCH DAMAGE.

4.0.30319.42000
10/9/2018 5:25:08 AM

*/

#include "pch.h"
#include "Lina_EventHandler.h"  

Lina_EventHandler::Lina_EventHandler()
{

}

/* ACITON SUBSCRIPTION OVERLOADS */

void Lina_EventHandler::SubscribeToAction(ActionType at, std::function<void()>const& cb)
{
	// Init the handler.
	Lina_ActionHandler<> handler(at);
	handler.SetUseNoParamCallback(true);
	handler.SetNoParamCallback(cb);

	// Push it a shared ptr to the handler to the list.
	m_Handlers.push_back(std::make_shared<Lina_ActionHandler<>>(handler));

	// Get the shared ptr pushed just now.
	std::list<std::shared_ptr<Lina_ActionHandlerBase>>::iterator iter = std::prev(m_Handlers.end());

	// construct a weakptr out of the shared.
	std::weak_ptr<Lina_ActionHandlerBase> wptr = *iter;

	// Call the subscription to subscribe to relevant dispatcher.
	Subscribe(at, wptr);
}

template<typename T>
void Lina_EventHandler::SubscribeToAction(ActionType at, std::function<void(T)>const& cbp)
{
	// Init the handler.
	Lina_ActionHandler<T> handler(at);
	handler.SetUseParamCallback(true);
	handler.SetParamCallback(cbp);

	// Push it a shared ptr to the handler to the list.
	m_Handlers.push_back(std::make_shared<Lina_ActionHandler<T>>(handler));

	// Get the shared ptr pushed just now.
	std::list<std::shared_ptr<Lina_ActionHandlerBase>>::iterator iter = std::prev(m_Handlers.end());

	// construct a weakptr out of the shared.
	std::weak_ptr<Lina_ActionHandlerBase> wptr = *iter;

	// Call the subscription to subscribe to relevant dispatcher.
	Subscribe(at, wptr);
}

template<typename T>
void Lina_EventHandler::SubscribeToAction(ActionType at, T* binding)
{
	// Init the handler.
	Lina_ActionHandler<T> handler(at);
	handler.SetUseBinding(true);
	handler.SetBinding(binding);

	// Push it a shared ptr to the handler to the list.
	m_Handlers.push_back(std::make_shared<Lina_ActionHandler<T>>(handler));

	// Get the shared ptr pushed just now.
	std::list<std::shared_ptr<Lina_ActionHandlerBase>>::iterator iter = std::prev(m_Handlers.end());

	// construct a weakptr out of the shared.
	std::weak_ptr<Lina_ActionHandlerBase> wptr = *iter;

	// Call the subscription to subscribe to relevant dispatcher.
	Subscribe(at, wptr);
}

template<typename T>
void Lina_EventHandler::SubscribeToAction(ActionType at, T condition, std::function<void()>const& cb)
{
	// Init the handler.
	Lina_ActionHandler<T> handler(at);
	handler.SetUseNoParamCallback(true);
	handler.SetUseCondition(true);
	handler.SetNoParamCallback(cb);
	handler.SetCondition(condition);

	// Push it a shared ptr to the handler to the list.
	m_Handlers.push_back(std::make_shared<Lina_ActionHandler<T>>(handler));

	// Get the shared ptr pushed just now.
	std::list<std::shared_ptr<Lina_ActionHandlerBase>>::iterator iter = std::prev(m_Handlers.end());

	// construct a weakptr out of the shared.
	std::weak_ptr<Lina_ActionHandlerBase> wptr = *iter;

	// Call the subscription to subscribe to relevant dispatcher.
	Subscribe(at, wptr);
}

template<typename T>
void Lina_EventHandler::SubscribeToAction(ActionType at, T condition, std::function<void(T)>const& cbp)
{
	// Init the handler.
	Lina_ActionHandler<T> handler(at);
	handler.SetUseParamCallback(true);
	handler.SetUseCondition(true);
	handler.SetParamCallback(cbp);
	handler.SetCondition(condition);

	// Push it a shared ptr to the handler to the list.
	m_Handlers.push_back(std::make_shared<Lina_ActionHandler<T>>(handler));

	// Get the shared ptr pushed just now.
	std::list<std::shared_ptr<Lina_ActionHandlerBase>>::iterator iter = std::prev(m_Handlers.end());

	// construct a weakptr out of the shared.
	std::weak_ptr<Lina_ActionHandlerBase> wptr = *iter;

	// Call the subscription to subscribe to relevant dispatcher.
	Subscribe(at, wptr);
}

template<typename T>
void Lina_EventHandler::SubscribeToAction(ActionType at, T condition, T* binding)
{
	// Init the handler.
	Lina_ActionHandler<T> handler(at);
	handler.SetUseBinding(true);
	handler.SetUseCondition(true);
	handler.SetBinding(binding);
	handler.SetCondition(condition);

	// Push it a shared ptr to the handler to the list.
	m_Handlers.push_back(std::make_shared<Lina_ActionHandler<T>>(handler));

	// Get the shared ptr pushed just now.
	std::list<std::shared_ptr<Lina_ActionHandlerBase>>::iterator iter = std::prev(m_Handlers.end());

	// construct a weakptr out of the shared.
	std::weak_ptr<Lina_ActionHandlerBase> wptr = *iter;

	// Call the subscription to subscribe to relevant dispatcher.
	Subscribe(at, wptr);
}

template<typename T>
void Lina_EventHandler::SubscribeToAction(ActionType at, std::function<void()>const& cb, std::function<void(T)>const& cbp)
{
	// Init the handler.
	Lina_ActionHandler<T> handler(at);
	handler.SetUseNoParamCallback(true);
	handler.SetUseParamCallback(true);
	handler.SetNoParamCallback(cb);
	handler.SetParamCallback(cbp);

	// Push it a shared ptr to the handler to the list.
	m_Handlers.push_back(std::make_shared<Lina_ActionHandler<T>>(handler));

	// Get the shared ptr pushed just now.
	std::list<std::shared_ptr<Lina_ActionHandlerBase>>::iterator iter = std::prev(m_Handlers.end());

	// construct a weakptr out of the shared.
	std::weak_ptr<Lina_ActionHandlerBase> wptr = *iter;

	// Call the subscription to subscribe to relevant dispatcher.
	Subscribe(at, wptr);
}

template<typename T>
void Lina_EventHandler::SubscribeToAction(ActionType at, std::function<void()>const& cb, T* binding)
{
	// Init the handler.
	Lina_ActionHandler<T> handler(at);
	handler.SetUseNoParamCallback(true);
	handler.SetUseBinding(true);
	handler.SetNoParamCallback(cb);
	handler.SetBinding(binding);

	// Push it a shared ptr to the handler to the list.
	m_Handlers.push_back(std::make_shared<Lina_ActionHandler<T>>(handler));

	// Get the shared ptr pushed just now.
	std::list<std::shared_ptr<Lina_ActionHandlerBase>>::iterator iter = std::prev(m_Handlers.end());

	// construct a weakptr out of the shared.
	std::weak_ptr<Lina_ActionHandlerBase> wptr = *iter;

	// Call the subscription to subscribe to relevant dispatcher.
	Subscribe(at, wptr);
}

template<typename T>
void Lina_EventHandler::SubscribeToAction(ActionType at, std::function<void(T)>const& cbp, T* binding)
{
	// Init the handler.
	Lina_ActionHandler<T> handler(at);
	handler.SetUseParamCallback(true);
	handler.SetUseBinding(true);
	handler.SetParamCallback(cbp);
	handler.SetBinding(binding);

	// Push it a shared ptr to the handler to the list.
	m_Handlers.push_back(std::make_shared<Lina_ActionHandler<T>>(handler));

	// Get the shared ptr pushed just now.
	std::list<std::shared_ptr<Lina_ActionHandlerBase>>::iterator iter = std::prev(m_Handlers.end());

	// construct a weakptr out of the shared.
	std::weak_ptr<Lina_ActionHandlerBase> wptr = *iter;

	// Call the subscription to subscribe to relevant dispatcher.
	Subscribe(at, wptr);
}

template<typename T>
void Lina_EventHandler::SubscribeToAction(ActionType at, T condition, std::function<void()>const& cb, std::function<void(T)>const& cbp)
{
	// Init the handler.
	Lina_ActionHandler<T> handler(at);
	handler.SetUseCondition(true);
	handler.SetUseNoParamCallback(true);
	handler.SetUseParamCallback(true);
	handler.SetCondition(condition);
	handler.SetNoParamCallback(cb);
	handler.SetParamCallback(cbp);

	// Push it a shared ptr to the handler to the list.
	m_Handlers.push_back(std::make_shared<Lina_ActionHandler<T>>(handler));

	// Get the shared ptr pushed just now.
	std::list<std::shared_ptr<Lina_ActionHandlerBase>>::iterator iter = std::prev(m_Handlers.end());

	// construct a weakptr out of the shared.
	std::weak_ptr<Lina_ActionHandlerBase> wptr = *iter;

	// Call the subscription to subscribe to relevant dispatcher.
	Subscribe(at, wptr);
}

template<typename T>
void Lina_EventHandler::SubscribeToAction(ActionType at, T condition, std::function<void(T)>const& cbp, T* binding)
{
	// Init the handler.
	Lina_ActionHandler<T> handler(at);
	handler.SetUseCondition(true);
	handler.SetUseBinding(true);
	handler.SetUseParamCallback(true);
	handler.SetCondition(condition);
	handler.SetBinding(binding);
	handler.SetParamCallback(cbp);

	// Push it a shared ptr to the handler to the list.
	m_Handlers.push_back(std::make_shared<Lina_ActionHandler<T>>(handler));

	// Get the shared ptr pushed just now.
	std::list<std::shared_ptr<Lina_ActionHandlerBase>>::iterator iter = std::prev(m_Handlers.end());

	// construct a weakptr out of the shared.
	std::weak_ptr<Lina_ActionHandlerBase> wptr = *iter;

	// Call the subscription to subscribe to relevant dispatcher.
	Subscribe(at, wptr);
}

template<typename T>
void Lina_EventHandler::SubscribeToAction(ActionType at, T condition, std::function<void()>const& cb, T* binding)
{
	// Init the handler.
	Lina_ActionHandler<T> handler(at);
	handler.SetUseCondition(true);
	handler.SetUseNoParamCallback(true);
	handler.SetUseBinding(true);
	handler.SetCondition(condition);
	handler.SetNoParamCallback(cb);
	handler.SetBinding(binding);

	// Push it a shared ptr to the handler to the list.
	m_Handlers.push_back(std::make_shared<Lina_ActionHandler<T>>(handler));

	// Get the shared ptr pushed just now.
	std::list<std::shared_ptr<Lina_ActionHandlerBase>>::iterator iter = std::prev(m_Handlers.end());

	// construct a weakptr out of the shared.
	std::weak_ptr<Lina_ActionHandlerBase> wptr = *iter;

	// Call the subscription to subscribe to relevant dispatcher.
	Subscribe(at, wptr);
}

template<typename T>
void Lina_EventHandler::SubscribeToAction(ActionType at, std::function<void()>const& cb, std::function<void(T)>const& cbp, T* binding)
{
	// Init the handler.
	Lina_ActionHandler<T> handler(at);
	handler.SetUseNoParamCallback(true);
	handler.SetUseParamCallback(true);
	handler.SetUseBinding(true);
	handler.SetNoParamCallback(cb);
	handler.SetParamCallback(cbp);
	handler.SetBinding(binding);

	// Push it a shared ptr to the handler to the list.
	m_Handlers.push_back(std::make_shared<Lina_ActionHandler<T>>(handler));

	// Get the shared ptr pushed just now.
	std::list<std::shared_ptr<Lina_ActionHandlerBase>>::iterator iter = std::prev(m_Handlers.end());

	// construct a weakptr out of the shared.
	std::weak_ptr<Lina_ActionHandlerBase> wptr = *iter;

	// Call the subscription to subscribe to relevant dispatcher.
	Subscribe(at, wptr);
}

template<typename T>
void Lina_EventHandler::SubscribeToAction(ActionType at, T condition, std::function<void()>const& cb, std::function<void(T)>const& cbp, T* binding)
{
	// Init the handler.
	Lina_ActionHandler<T> handler(at);
	handler.SetUseCondition(true);
	handler.SetUseNoParamCallback(true);
	handler.SetUseParamCallback(true);
	handler.SetCondition(condition);
	handler.SetNoParamCallback(cb);
	handler.SetParamCallback(cbp);

	// Push it a shared ptr to the handler to the list.
	m_Handlers.push_back(std::make_shared<Lina_ActionHandler<T>>(handler));

	// Get the shared ptr pushed just now.
	std::list<std::shared_ptr<Lina_ActionHandlerBase>>::iterator iter = std::prev(m_Handlers.end());

	// construct a weakptr out of the shared.
	std::weak_ptr<Lina_ActionHandlerBase> wptr = *iter;

	// Call the subscription to subscribe to relevant dispatcher.
	Subscribe(at, wptr);
}

void Lina_EventHandler::Subscribe(ActionType at, std::weak_ptr<Lina_ActionHandlerBase> wptr)
{
	if (at == ActionType::KeyPressed || at == ActionType::KeyReleased || at == ActionType::MouseButtonPressed || at == ActionType::MouseButtonReleased || 
		at == ActionType::MouseMotionX || at == ActionType::MouseMotionY)
	{
		Lina_CoreMessageBus::Instance().GetInputDispatcher()->SubscribeHandler(wptr);
	}
}

float Lina_EventHandler::GetMouseX()
{
	return Lina_CoreMessageBus::Instance().GetInputEngine()->GetMouseAxis(0);

}

