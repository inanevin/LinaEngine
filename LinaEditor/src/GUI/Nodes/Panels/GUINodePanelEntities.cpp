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

#include "GUI/Nodes/Panels/GUINodePanelEntities.hpp"
#include "GUI/Nodes/Layouts/GUINodeLayoutVertical.hpp"
#include "GUI/Nodes/Widgets/GUINodeSelection.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "System/ISystem.hpp"
#include "Core/Editor.hpp"
#include "Core/Theme.hpp"
#include "World/Level/LevelManager.hpp"
#include "World/Level/Level.hpp"
#include "World/Core/EntityWorld.hpp"
#include "Data/CommonData.hpp"

namespace Lina::Editor
{
	GUINodePanelEntities::GUINodePanelEntities(GUIDrawerBase* drawer, int drawOrder, EditorPanel panelType, const String& title, GUINodeDockArea* parentDockArea) : GUINodePanel(drawer, drawOrder, panelType, title, parentDockArea)
	{
		m_levelManager = m_editor->GetSystem()->CastSubsystem<LevelManager>(SubsystemType::LevelManager);
		m_payloadMask.Set(EPL_Entity);
		m_editor->GetSystem()->AddListener(this);
		m_layout = new GUINodeLayoutVertical(drawer, m_drawOrder);
		m_layout->SetFixedHorizontalSize(true);
		AddChildren(m_layout);
		m_editor->AddListener(this);

		if (m_levelManager->GetCurrentLevel() != nullptr)
		{
			m_world = m_levelManager->GetCurrentLevel()->GetWorld();
			m_world->AddListener(this);
			GetLevelEntityMeta();
			RefreshEntityList();
		}
	}

	GUINodePanelEntities::~GUINodePanelEntities()
	{
		m_editor->RemoveListener(this);

		if (m_world)
			m_world->RemoveListener(this);

		m_editor->GetSystem()->RemoveListener(this);
	}

	void GUINodePanelEntities::Draw(int threadID)
	{
		GUINodePanel::Draw(threadID);

		// Handle entity parent-child switching
		{
			if (m_entityDropData.active)
			{
				// Reorder drop
				if (m_entityDropData.dropState == 1)
				{
					m_entityDropData.srcEntity->RemoveFromParent();

					auto targetParent = m_entityDropData.targetEntity->GetParent();

					if (targetParent != nullptr)
						targetParent->AddChild(m_entityDropData.srcEntity);

					ClearEntityList();
					for (auto e : m_orderedEntities)
						AddEntityToTree(e, nullptr);

					auto* srcNode	 = m_entityState[m_entityDropData.srcEntity].node;
					auto* targetNode = m_entityState[m_entityDropData.targetEntity].node;

					// if (targetParent)
					//{
					//	auto* parentNode = m_entityState[targetParent].node;
					//	parentNode->PlaceChildAfter(srcNode, targetNode);
					// }
					// else
					//{
					//	auto a = static_cast<GUINode*>(srcNode);
					//	auto b = static_cast<GUINode*>(targetNode);
					//	Internal::PlaceAfter(m_layout->GetChildren(), a, b);
					// }
				}
				else
				{
					// If the entity we are dropping is the parent of the target, free target first.
					if (m_entityDropData.srcEntity->HasChildInTree(m_entityDropData.targetEntity))
					{
						auto* parentOfSrc = m_entityDropData.srcEntity->GetParent();
						m_entityDropData.targetEntity->RemoveFromParent();

						if (parentOfSrc != nullptr)
							parentOfSrc->AddChild(m_entityDropData.targetEntity);
					}

					m_entityDropData.targetEntity->AddChild(m_entityDropData.srcEntity);

					RefreshEntityList();
				}

				m_entityDropData = EntityDropData();
			}
		}

		const float padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		m_layout->SetPos(m_rect.pos + Vector2(0.0f, padding * 0.5f));
		m_layout->SetSize(m_rect.size);
		m_layout->Draw(threadID);
	}

	void GUINodePanelEntities::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
		if (eventType & EVS_LevelInstalled)
		{
			m_world = m_levelManager->GetCurrentLevel()->GetWorld();
			m_world->AddListener(this);
			GetLevelEntityMeta();
			RefreshEntityList();
		}
		else if (eventType & EVS_PreLevelUninstall)
		{
			m_world->RemoveListener(this);
			m_world			 = nullptr;
			m_selectedEntity = nullptr;
			m_levelEntityMeta.clear();
			ClearEntityList();
		}
	}

	void GUINodePanelEntities::OnGameEvent(GameEvent eventType, const Event& ev)
	{
		if (eventType & EVG_EntityCreated)
			RefreshEntityList();
		else if (eventType & EVG_EntityDestroyed)
			RefreshEntityList();
	}

	void GUINodePanelEntities::OnEditorEvent(EditorEvent eventType, const Event& ev)
	{
		if (eventType & EVE_SelectEntity)
		{
			if (m_selectedEntity != nullptr)
				m_entityState[m_selectedEntity].node->SetSelected(false);

			m_selectedEntity = static_cast<Entity*>(ev.pParams[0]);
			m_entityState[m_selectedEntity].node->SetSelected(true);
		}
		else if (eventType & EVE_UnselectCurrentEntity)
		{
			m_entityState[m_selectedEntity].node->SetSelected(false);
			m_selectedEntity = nullptr;
		}
		else if (eventType & EVE_PreSavingCurrentLevel)
		{
			OStream& outStream = m_levelManager->GetCurrentLevel()->GetOutRuntimeUserStream();
			HashMapSerialization::SaveToStream_OBJ(outStream, m_levelEntityMeta);
		}
	}

	void GUINodePanelEntities::OnPayloadAccepted()
	{
		Entity* draggedEntity = static_cast<Entity*>(m_editor->GetPayloadManager().GetCurrentPayloadMeta().data);
		draggedEntity->RemoveFromParent();
		RefreshEntityList();
	}

	void GUINodePanelEntities::RefreshEntityList()
	{
		// Clear layout
		ClearEntityList();

		Vector<Entity*> entities;
		m_world->GetAllEntities(entities);

		m_orderedEntities.clear();
		for (auto e : entities)
		{
			if (e->GetParent() == nullptr)
				m_orderedEntities.push_back(e);
		}

		for (auto e : m_orderedEntities)
			AddEntityToTree(e, nullptr);
	}

	void GUINodePanelEntities::ClearEntityList()
	{
		for (auto [e, n] : m_entityState)
			n.expandState = n.node->GetIsExpanded();

		RemoveChildren(m_layout);
		delete m_layout;
		m_layout = new GUINodeLayoutVertical(m_drawer, m_drawOrder);
		m_layout->SetFixedHorizontalSize(true);
		AddChildren(m_layout);
	}

	void GUINodePanelEntities::AddEntityToTree(Entity* entity, GUINodeSelection* parentNode)
	{
		// create new selection
		GUINodeSelection* myTreeItem = new GUINodeSelection(m_drawer, m_drawOrder);
		m_entityState[entity].node	 = myTreeItem;

		myTreeItem->SetCallbackClicked(BIND(&GUINodePanelEntities::OnClickedSelection, this, std::placeholders::_1));
		myTreeItem->SetCallbackDetached(BIND(&GUINodePanelEntities::OnSelectionDetached, this, std::placeholders::_1, std::placeholders::_2));
		myTreeItem->SetCallbackPayloadAccepted(BIND(&GUINodePanelEntities::OnSelectionPayloadAccepted, this, std::placeholders::_1, std::placeholders::_2));
		myTreeItem->SetTitle(entity->GetName().c_str());
		myTreeItem->GetPayloadMask().Set(EPL_Entity);
		myTreeItem->SetIsExpanded(m_entityState[entity].expandState);

		if (parentNode != nullptr)
			parentNode->AddChildSelection(myTreeItem);
		else
			m_layout->AddChildren(myTreeItem);

		const auto& children = entity->GetChildren();
		for (const auto& c : children)
			AddEntityToTree(c, myTreeItem);
	}

	void GUINodePanelEntities::OnClickedSelection(GUINode* selectionNode)
	{
		Entity* target = FindEntityFromNode(selectionNode);

		m_editor->GetCommandManager().CreateCommand_SelectEntity(m_selectedEntity, target);
	}

	void GUINodePanelEntities::OnSelectionDetached(GUINodeSelection* selection, const Vector2& delta)
	{
		const float padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		m_editor->GetPayloadManager().CreatePayload(PayloadType::EPL_Entity, selection->GetLastCalculatedTextSize() + Vector2(padding * 2, padding), Vector2i(delta), FindEntityFromNode(selection));
	}

	void GUINodePanelEntities::OnSelectionPayloadAccepted(GUINode* selection, void* userData)
	{
		Entity* target		  = FindEntityFromNode(selection);
		Entity* draggedEntity = static_cast<Entity*>(m_editor->GetPayloadManager().GetCurrentPayloadMeta().data);

		if (draggedEntity == target)
			return;

		m_entityDropData.active		  = true;
		m_entityDropData.srcEntity	  = draggedEntity;
		m_entityDropData.targetEntity = target;
		m_entityDropData.dropState	  = *static_cast<int*>(userData);
	}

	Entity* GUINodePanelEntities::FindEntityFromNode(GUINode* node)
	{
		for (auto [ent, state] : m_entityState)
		{
			if (node == state.node)
				return ent;
		}
		return nullptr;
	}

	void GUINodePanelEntities::GetLevelEntityMeta()
	{
		auto& loadedStream = m_levelManager->GetCurrentLevel()->GetLoadedRuntimeUserStream();

		if (loadedStream.GetSize() != 0)
			HashMapSerialization::LoadFromStream_OBJ(loadedStream, m_levelEntityMeta);
	}

	void GUINodePanelEntities::EntityMeta::SaveToStream(OStream& stream)
	{
		stream << expandState;
		stream << localOrder;
	}

	void GUINodePanelEntities::EntityMeta::LoadFromStream(IStream& stream)
	{
		stream >> expandState;
		stream >> localOrder;
	}
} // namespace Lina::Editor
