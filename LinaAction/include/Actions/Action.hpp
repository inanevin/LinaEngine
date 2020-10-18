/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
class: Action

Defines action handler base and derived classes which are responsible for holding an action type,
setting up conditions if desired, checking conditions and executing the function objects assigned
to them.

Timestamp: 1/6/2019 5:41:20 AM
*/

#pragma once

#ifndef Action_HPP
#define Action_HPP

#include "Core/Common.hpp"
#include <functional>

namespace LinaEngine::Action
{
	enum ActionType
	{
		// Input actions
		KeyPressed = 0,
		KeyReleased,
		MouseButtonPressed,
		MouseButtonReleased,
		InputActionsStartIndex = KeyPressed,
		InputActionsEndIndex = MouseButtonReleased,

		// Engine actions
		MessageLogged,
		PostSceneDraw,
		WindowClosed,
		WindowResized,
		EngineActionsStartIndex = MessageLogged,
		EngineActionsEndIndex = WindowResized,

		// Editor actions
		TextureSelected,
		EntitySelected,
		MaterialSelected,
		MeshSelected,
		Unselect,
		MenuItemClicked,
		EditorActionsStartIndex = TextureSelected,
		EditorActionsEndIndex = MenuItemClicked
	};

	class ActionHandlerBase
	{

	public:

		virtual ~ActionHandlerBase() {};
		ActionType GetActionType() const { return m_actionType; }
		bool GetUseCondition() const { return m_useCondition; }
		size_t GetHashedID() const { return m_hashedID; }

	protected:

		ActionHandlerBase(ActionType at, size_t id) : m_actionType(at), m_hashedID(id) {};

	protected:

		size_t m_hashedID = 0;
		bool m_useCondition = false;
		ActionType m_actionType;

	};

	template<typename T>
	class ActionHandler : public ActionHandlerBase
	{

	public:
	
		virtual ~ActionHandler() {};

		void SetCondition(const T& cond)
		{ 
			m_condition = cond;
			m_useCondition = true; 
		}

		void SetCallback(const std::function<void(T)>& cb) { m_callback = cb; }

		// Checks condition if one is used and invokes the callback.
		void ControlExecute(const T& data) 
		{ 
			if (!m_useCondition || ( m_useCondition && m_condition == data))
				m_callback(data);
		}

	private:

		friend class ActionDispatcher;
		ActionHandler(ActionType at, size_t id) : ActionHandlerBase(at, id) {};

	private:

		std::function<void(T)> m_callback;
		T m_condition;
	};


	
}


#endif