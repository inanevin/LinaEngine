/*
Author: Inan Evin
www.inanevin.com

MIT License

Lina Engine, Copyright (c) 2018 Inan Evin

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following Ts:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

4.0.30319.42000
10/9/2018 3:36:41 AM

*/

#pragma once

#ifndef Lina_EventSubscriber_HPP
#define Lina_EventSubscriber_HPP

#include "Lina_CoreMessageBus.hpp"
#include "Events/Lina_Actions.hpp"
#include "Input/Lina_InputEngine.hpp"


class Lina_ObjectHandler
{
public:

	Lina_ObjectHandler() {};
	~Lina_ObjectHandler() {};

	void Initialize()
	{
		// Retrieve the input engine pointer from the message bus.
		inputEngine = Lina_CoreMessageBus::Instance().GetInputEngine();
		inputDispatcher = inputEngine->GetInputDispatcher();
	}

#pragma region ActionSubscriptionOverloads

	void SubscribeToAction(ActionType at, std::function<void()>const&& cb)
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
	void SubscribeToAction(ActionType at, std::function<void(T)>const&& cbp)
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
	void SubscribeToAction(ActionType at, T* binding)
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
	void SubscribeToAction(ActionType at, T condition, std::function<void()>const&& cb)
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
	void SubscribeToAction(ActionType at, T condition, std::function<void(T)>const&& cbp)
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
	void SubscribeToAction(ActionType at, T condition, T* binding)
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
	void SubscribeToAction(ActionType at, std::function<void()>const&& cb, std::function<void(T)>const&& cbp)
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
	void SubscribeToAction(ActionType at, std::function<void()>const&& cb, T* binding)
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
	void SubscribeToAction(ActionType at, std::function<void(T)>const&& cbp, T* binding)
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
	void SubscribeToAction(ActionType at, T condition, std::function<void()>const&& cb, std::function<void(T)>const&& cbp)
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
	void SubscribeToAction(ActionType at, T condition, std::function<void(T)>const&& cbp, T* binding)
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
	void SubscribeToAction(ActionType at, T condition, std::function<void()>const&& cb, T* binding)
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
	void SubscribeToAction(ActionType at, std::function<void()>const&& cb, std::function<void(T)>const&& cbp, T* binding)
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
	void SubscribeToAction(ActionType at, T condition, std::function<void()>const&& cb, std::function<void(T)>const&& cbp, T* binding)
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

	void Subscribe(ActionType at, std::weak_ptr<Lina_ActionHandlerBase> wptr)
	{
		if (at == ActionType::KeyPressed || at == ActionType::KeyReleased || at == ActionType::MouseButtonPressed || at == ActionType::MouseButtonReleased ||
			at == ActionType::MouseMotionX || at == ActionType::MouseMotionY || at == ActionType::SDLQuit)
		{
			inputDispatcher->SubscribeHandler(wptr);
		}
	}

#pragma endregion

#pragma region InputPollers

	bool GetKey(SDL_Scancode k)
	{
		return inputEngine->GetKey(k);
	}

	bool GetKeyDown(SDL_Scancode k)
	{
		return inputEngine->GetKeyDown(k);
	}

	bool GetKeyUp(SDL_Scancode k)
	{
		return inputEngine->GetKeyUp(k);
	}
	// Returns the smoothed delta mouse x value.
	float GetMouseX()
	{
		return inputEngine->GetMouseX();
	}

	// Returns the smoothed delta mouse y value.
	float GetMouseY()
	{
		return inputEngine->GetMouseY();
	}

	// Returns the raw delta mouse x value.
	float GetRawMouseX()
	{
		return inputEngine->GetRawMouseX();
	}

	// Returns the raw delta mouse y value.
	float GetRawMouseY()
	{
		return inputEngine->GetRawMouseY();
	}

	bool GetMouseButton(int button)
	{
		return inputEngine->GetMouseButton(button);
	}

	bool GetMouseButtonDown(int button)
	{
		return inputEngine->GetMouseButtonDown(button);
	}

	bool GetMouseButtonUp(int button)
	{
		return inputEngine->GetMouseButtonUp(button);
	}

#pragma endregion

private:

	std::list<std::shared_ptr<Lina_ActionHandlerBase>> m_Handlers;
	Lina_InputEngine* inputEngine;
	Lina_ActionDispatcher* inputDispatcher;
};


#endif