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
#include "Core/World/EntityWorld.hpp"
#include "Core/GUI/Widgets/Compound/FileMenu.hpp"
#include "Editor/Editor.hpp"

namespace Lina
{
	class WorldManager;
	class Entity;
	class DirectionalLayout;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;
	class PanelEntities : public Panel, public EntityWorldListener, public FileMenuListener
	{
	public:
		PanelEntities() : Panel(PanelType::Entities, 0){};
		virtual ~PanelEntities() = default;

		virtual void Construct() override;
		virtual void Destruct() override;
		virtual void PreTick() override;
		virtual void Tick(float dt) override;

		virtual bool OnFileMenuItemClicked(FileMenu* filemenu, StringID sid, void* userData) override;
		virtual void OnFileMenuGetItems(FileMenu* filemenu, StringID sid, Vector<FileMenuItem::Data>& outData, void* userData) override;

	private:
		Editor*		  m_editor		 = nullptr;
		WorldManager* m_worldManager = nullptr;
		EntityWorld*  m_world		 = nullptr;
	};

	LINA_REFLECTWIDGET_BEGIN(PanelEntities, Editor)
	LINA_REFLECTWIDGET_END(PanelEntities)

} // namespace Lina::Editor
