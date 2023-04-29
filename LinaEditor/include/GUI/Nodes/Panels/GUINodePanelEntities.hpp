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
#include "Serialization/ISerializable.hpp"

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

		struct EntityState
		{
			GUINodeSelection* node		  = nullptr;
			bool			  expandState = false;
		};

		struct EntityMeta : public ISerializable
		{
			bool   expandState = false;
			uint32 localOrder  = 0;

			// Inherited via ISerializable
			virtual void SaveToStream(OStream& stream) override;
			virtual void LoadFromStream(IStream& stream) override;
		};

	public:
		GUINodePanelEntities(GUIDrawerBase* drawer, int drawOrder, EditorPanel panelType, const String& title, GUINodeDockArea* parentDockArea);
		virtual ~GUINodePanelEntities();
		virtual void Draw(int threadID);
		virtual void OnSystemEvent(SystemEvent eventType, const Event& ev);
		virtual void OnGameEvent(GameEvent eventType, const Event& ev);
		virtual void OnEditorEvent(EditorEvent eventType, const Event& ev);
		virtual void OnPayloadAccepted() override;

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
		void	RefreshEntityList();
		void	ClearEntityList();
		void	AddEntityToTree(Entity* entity, GUINodeSelection* parentNode);
		void	OnClickedSelection(GUINode* selectionNode);
		void	OnSelectionDetached(GUINodeSelection* selection, const Vector2& delta);
		void	OnSelectionPayloadAccepted(GUINode* selection, void* userData);
		Entity* FindEntityFromNode(GUINode* node);

		void GetLevelEntityMeta();

	private:
		HashMap<uint32, EntityMeta>	  m_levelEntityMeta;
		Vector<Entity*>				  m_orderedEntities;
		HashMap<Entity*, EntityState> m_entityState;
		EntityDropData				  m_entityDropData;
		Entity*						  m_selectedEntity = nullptr;
		GUINodeLayoutVertical*		  m_layout		   = nullptr;
		EntityWorld*				  m_world		   = nullptr;
		LevelManager*				  m_levelManager   = nullptr;
	};
} // namespace Lina::Editor

#endif
