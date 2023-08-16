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

#ifndef GUINodePanelEntities_HPP
#define GUINodePanelEntities_HPP

#include "GUI/Nodes/Panels/GUINodePanel.hpp"
#include "Event/IGameEventListener.hpp"
#include "Event/ISystemEventListener.hpp"
#include "Event/IEditorEventListener.hpp"
#include "Data/HashMap.hpp"

namespace Lina
{
	class LevelManager;
	class EntityWorld;
	class Entity;
} // namespace Lina

namespace Lina::Editor
{
	class GUINodeLayoutVertical;
	class GUINodeSelection;
	class GUINodeFMPopup;
	class GUINodeFMPopupElementToggle;

	class GUINodePanelEntities : public GUINodePanel, public ISystemEventListener, public IGameEventListener, public IEditorEventListener
	{

	private:
		struct EntityDropData
		{
			bool	active		 = false;
			int		dropState	 = 0;
			Entity* targetEntity = nullptr;
			Entity* srcEntity	 = nullptr;
		};

	public:
		GUINodePanelEntities(GUIDrawerBase* drawer, int drawOrder, EditorPanel panelType, const String& title, GUINodeDockArea* parentDockArea);
		virtual ~GUINodePanelEntities();
		virtual void Draw(int threadID);
		virtual void OnSystemEvent(SystemEvent eventType, const Event& ev);
		virtual void OnGameEvent(GameEvent eventType, const Event& ev);
		virtual void OnEditorEvent(EditorEvent eventType, const Event& ev);
		virtual void OnPayloadAccepted() override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction action) override;

		virtual Bitmask32 GetSystemEventMask()
		{
			return EVS_LevelInstalled | EVS_PreLevelUninstall;
		}

		virtual Bitmask32 GetGameEventMask()
		{
			return EVG_EntityCreated | EVG_EntityDestroyed;
		}

		virtual Bitmask32 GetEditorEventMask()
		{
			return EVE_SelectEntity | EVE_UnselectCurrentEntity | EVE_PreSavingCurrentLevel;
		}

	private:
		void RecreateEntityHierarchy();
		void CreateHierarchyForList(const Vector<Entity*>& list, GUINodeSelection* parent);
		void ClearEntityHierarchy();
		void SaveHierarchy(OStream& outStream, const Vector<GUINodeSelection*>& items);
		void LoadHierarchy(IStream& inStream, GUINodeSelection* parent);

		void			  OnClickedSelection(GUINode* selectionNode);
		void			  OnSelectionDetached(GUINodeSelection* selection, const Vector2& delta);
		void			  OnSelectionPayloadAccepted(GUINode* selection, void* userData);
		GUINodeSelection* CreateSelectionForEntity(Entity* e);

		void LoadLevelMeta();
		void OnPressedContextItem(GUINode* node);

	private:
		GUINodeFMPopup*						m_contextMenuSelection	  = nullptr;
		GUINodeFMPopupElementToggle*		m_contextMenuBoldToggle	  = nullptr;
		GUINodeFMPopupElementToggle*		m_contextMenuFolderToggle = nullptr;
		HashMap<GUINodeSelection*, Entity*> m_nodeToEntityMap;
		EntityDropData						m_entityDropData;
		Entity*								m_selectedEntity = nullptr;
		GUINodeLayoutVertical*				m_layout		 = nullptr;
		EntityWorld*						m_world			 = nullptr;
		LevelManager*						m_levelManager	 = nullptr;
	};
} // namespace Lina::Editor

#endif
