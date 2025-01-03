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

#include "EditorAction.hpp"
#include "Core/World/CommonWorld.hpp"
#include "Common/Data/String.hpp"

namespace Lina
{
	class Entity;
	class EntityWorld;
}; // namespace Lina

namespace Lina::Editor
{
	class Editor;

	class EditorActionEntitySelection : public EditorAction
	{
	public:
		EditorActionEntitySelection()		   = default;
		virtual ~EditorActionEntitySelection() = default;

		static EditorActionEntitySelection* Create(Editor* editor, uint64 worldId, const Vector<Entity*>& entities, bool select, bool clearOthers, StringID source = 0);
		virtual void						Execute(Editor* editor, ExecType type) override;

	private:
		Vector<EntityID> m_entities	 = {};
		Vector<EntityID> m_selection = {};
		uint64			 m_worldId	 = 0;
		StringID		 m_src		 = 0;
		bool			 m_select	 = false;
		bool			 m_clear	 = false;
	};

	class EditorActionEntityTransform : public EditorAction
	{
	public:
		EditorActionEntityTransform()		   = default;
		virtual ~EditorActionEntityTransform() = default;

		static EditorActionEntityTransform* Create(Editor* editor, uint64 worldId, const Vector<Entity*>& entities, const Vector<Transformation>& previousTransforms);
		virtual void						Execute(Editor* editor, ExecType type) override;

	private:
		Vector<EntityID>	   m_entities = {};
		Vector<Transformation> m_prevTransforms;
		Vector<Transformation> m_currentTransforms;
		uint64				   m_worldId = 0;
	};

	class EditorActionEntitiesCreated : public EditorAction
	{
	public:
		EditorActionEntitiesCreated() = default;
		virtual ~EditorActionEntitiesCreated()
		{
			if (m_stream.GetCurrentSize() != 0)
				m_stream.Destroy();
		}

		static EditorActionEntitiesCreated* Create(Editor* editor, EntityWorld* world, const Vector<Entity*>& entities);
		virtual void						Execute(Editor* editor, ExecType type) override;

	private:
		OStream			 m_stream  = {};
		uint64			 m_worldId = 0;
		Vector<EntityID> m_guids;
	};

	class EditorActionEntityDelete : public EditorAction
	{
	public:
		EditorActionEntityDelete() = default;
		virtual ~EditorActionEntityDelete()
		{
			if (m_stream.GetCurrentSize() != 0)
				m_stream.Destroy();
		}

		static EditorActionEntityDelete* Create(Editor* editor, EntityWorld* world, const Vector<Entity*>& entities);
		virtual void					 Execute(Editor* editor, ExecType type) override;

	private:
		Vector<EntityID> m_guids;
		OStream			 m_stream  = {};
		uint64			 m_worldId = 0;
	};

	class EditorActionEntityParenting : public EditorAction
	{
	public:
		EditorActionEntityParenting()		   = default;
		virtual ~EditorActionEntityParenting() = default;

		static EditorActionEntityParenting* Create(Editor* editor, EntityWorld* world, const Vector<Entity*>& entities, Entity* newParent);
		virtual void						Execute(Editor* editor, ExecType type) override;

	private:
		Vector<EntityID> m_guids;
		Vector<EntityID> m_oldParentGUIDs;
		EntityID		 m_newParentGUID = 0;
		uint64			 m_worldId		 = 0;
	};

} // namespace Lina::Editor
