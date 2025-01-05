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

#include "Editor/Widgets/Panel/Panel.hpp"
#include "Editor/World/EditorWorldManager.hpp"
#include "Core/World/CommonWorld.hpp"

namespace Lina
{
	class EntityWorld;
	class WorldRenderer;
	class Entity;
	class EntityWorld;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;
	class WorldDisplayer;
	class EditorWorldRenderer;

	class PanelWorld : public Panel, public EditorWorldManagerListener
	{
	public:
		PanelWorld() : Panel(PanelType::World){};
		virtual ~PanelWorld() = default;

		virtual void Construct() override;
		virtual void Destruct() override;

		inline WorldDisplayer* GetDisplayer() const
		{
			return m_worldDisplayer;
		}

	protected:
		virtual void OnWorldManagerOpenedWorld(EditorWorldRenderer* wr) override;
		virtual void OnWorldManagerClosingWorld(EditorWorldRenderer* wr) override;
		virtual void OnWorldManagerEntitySelectionChanged(EntityWorld* w, const Vector<Entity*>& entities, StringID source) override;
		virtual void OnWorldManagerEntityHierarchyChanged(EntityWorld* w) override;

	private:
		Editor*			m_editor		 = nullptr;
		WorldDisplayer* m_worldDisplayer = nullptr;
		EntityWorld*	m_world			 = nullptr;
	};

	LINA_WIDGET_BEGIN(PanelWorld, Hidden)
	LINA_CLASS_END(PanelWorld)

} // namespace Lina::Editor
