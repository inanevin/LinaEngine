/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#pragma once

#ifndef EditorCommandEntity_HPP
#define EditorCommandEntity_HPP

#include "EditorCommand.hpp"
#include "Event/IGameEventListener.hpp"

namespace Lina
{
	class Entity;
	class EntityWorld;
} // namespace Lina
namespace Lina::Editor
{
	class EditorCommandSelectEntity : public EditorCommand, public IGameEventListener
	{
	public:
		EditorCommandSelectEntity(Editor* editor, Entity* previous, Entity* current);
		virtual ~EditorCommandSelectEntity();
		virtual void OnGameEvent(GameEvent eventType, const Event& ev) override;

		virtual Bitmask32 GetGameEventMask() override
		{
			return EVG_EntityDestroyed | EVG_End;
		}

		virtual void Execute(void* userData);
		virtual void Undo();

	private:
		EntityWorld* m_world	= nullptr;
		Entity*		 m_previous = nullptr;
		Entity*		 m_current	= nullptr;
	};
} // namespace Lina::Editor

#endif
