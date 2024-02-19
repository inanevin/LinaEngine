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
#include "Platform/LinaVGIncl.hpp"
#include "System/ISystem.hpp"
#include "Core/Editor.hpp"
#include "Commands/EditorCommandEntity.hpp"
#include "Core/Theme.hpp"
#include "World/Level/LevelManager.hpp"
#include "World/Level/Level.hpp"
#include "World/Core/EntityWorld.hpp"
#include "Data/CommonData.hpp"
#include "GUI/Nodes/Widgets/GUINodeFileMenu.hpp"
#include "LinaGX/Core/InputMappings.hpp"

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

		auto* colorPopup = new GUINodeFMPopup(m_drawer, FRONTER_DRAW_ORDER);
		colorPopup->AddDefault("None");
		colorPopup->AddDefault("Select");
		colorPopup->SetCallbackClicked(BIND(&GUINodePanelEntities::OnPressedContextItem, this, std::placeholders::_1));
		m_contextMenuSelection = new GUINodeFMPopup(m_drawer, FRONTER_DRAW_ORDER);
		m_contextMenuSelection->AddExpandable("Color", colorPopup);
		m_contextMenuBoldToggle = m_contextMenuSelection->AddToggle("Bold Font", false);
		m_contextMenuSelection->SetVisible(false);
		m_contextMenuSelection->SetCallbackClicked(BIND(&GUINodePanelEntities::OnPressedContextItem, this, std::placeholders::_1));
		AddChildren(m_contextMenuSelection);

		if (m_levelManager->GetCurrentLevel() != nullptr)
		{
			m_world = m_levelManager->GetCurrentLevel()->GetWorld();
			m_world->AddListener(this);
			LoadLevelMeta();
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
					auto* src			  = m_entityDropData.srcEntity;
					auto* target		  = m_entityDropData.targetEntity;
					auto* srcSelection	  = Internal::KeyFromValue(m_nodeToEntityMap, src);
					auto* targetSelection = Internal::KeyFromValue(m_nodeToEntityMap, target);

					if (src->GetParent())
					{
						auto* srcParentSelection = Internal::KeyFromValue(m_nodeToEntityMap, src->GetParent());
						srcParentSelection->RemoveChildSelection(srcSelection);
						src->RemoveFromParent();
					}
					else
						m_layout->RemoveChildren(srcSelection);

					auto targetParent = target->GetParent();

					if (targetParent != nullptr)
					{
						targetParent->AddChild(m_entityDropData.srcEntity);
						auto* targetParentSelection = Internal::KeyFromValue(m_nodeToEntityMap, targetParent);
						targetParentSelection->AddChildSelection(srcSelection);

						auto& selectionChildren = targetParentSelection->GetSelectionChildren();
						Internal::PlaceAfter(selectionChildren, srcSelection, targetSelection);
					}
					else
					{
						m_layout->AddChildren(srcSelection);
						srcSelection->SetXOffset(0);

						auto& children = m_layout->GetChildren();
						auto* n1	   = static_cast<GUINode*>(srcSelection);
						auto* n2	   = static_cast<GUINode*>(targetSelection);
						Internal::PlaceAfter(children, n1, n2);
					}
				}
				else
				{
					auto* src			  = m_entityDropData.srcEntity;
					auto* target		  = m_entityDropData.targetEntity;
					auto* srcSelection	  = Internal::KeyFromValue(m_nodeToEntityMap, src);
					auto* targetSelection = Internal::KeyFromValue(m_nodeToEntityMap, target);

					// Remove the selection node of the source entity from its parent.
					{
						if (src->GetParent() == nullptr)
						{
							auto n = static_cast<GUINode*>(srcSelection);
							m_layout->RemoveChildren(n);
						}
						else
						{
							auto* srcParent			 = src->GetParent();
							auto* srcParentSelection = Internal::KeyFromValue(m_nodeToEntityMap, srcParent);
							srcParentSelection->RemoveChildSelection(srcSelection);
						}
					}

					// If the entity we are dropping is the parent of the target, free target first.
					if (src->HasChildInTree(target))
					{
						auto* parentOfSrc	 = src->GetParent();
						auto* parentOfTarget = target->GetParent();

						if (parentOfTarget)
						{
							target->RemoveFromParent();
							auto* parentSelectionOfTarget = Internal::KeyFromValue(m_nodeToEntityMap, parentOfTarget);
							parentSelectionOfTarget->RemoveChildSelection(targetSelection);
						}

						// Make em siblings
						if (parentOfSrc != nullptr)
						{
							auto* parentSelectionOfSrc = Internal::KeyFromValue(m_nodeToEntityMap, parentOfSrc);
							parentSelectionOfSrc->AddChildSelection(targetSelection);
							parentOfSrc->AddChild(target);
						}
						else
						{
							m_layout->AddChildren(targetSelection);
							targetSelection->SetXOffset(0);
						}
					}

					target->AddChild(src);
					targetSelection->AddChildSelection(srcSelection);
				}

				m_entityDropData = EntityDropData();
			}
		}

		const float padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		m_layout->SetPos(m_rect.pos + Vector2(0.0f, padding * 0.5f));
		m_layout->SetSize(m_rect.size);
		m_layout->Draw(threadID);

		if (m_contextMenuSelection->GetIsVisible())
			m_contextMenuSelection->Draw(threadID);
	}

	void GUINodePanelEntities::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
		if (eventType & EVS_LevelInstalled)
		{
			m_world = m_levelManager->GetCurrentLevel()->GetWorld();
			m_world->AddListener(this);
			LoadLevelMeta();
		}
		else if (eventType & EVS_PreLevelUninstall)
		{
			m_world->RemoveListener(this);
			m_world			 = nullptr;
			m_selectedEntity = nullptr;
			ClearEntityHierarchy();
		}
	}

	void GUINodePanelEntities::OnGameEvent(GameEvent eventType, const Event& ev)
	{
		if (eventType & EVG_EntityCreated)
		{
		}
		else if (eventType & EVG_EntityDestroyed)
		{
		}
	}

	void GUINodePanelEntities::OnEditorEvent(EditorEvent eventType, const Event& ev)
	{
		if (eventType & EVE_SelectEntity)
		{
			if (m_selectedEntity != nullptr)
			{
				GUINodeSelection* key = Internal::KeyFromValue(m_nodeToEntityMap, m_selectedEntity);
				key->SetSelected(false);
			}

			m_selectedEntity = static_cast<Entity*>(ev.pParams[0]);

			GUINodeSelection* key = Internal::KeyFromValue(m_nodeToEntityMap, m_selectedEntity);
			key->SetSelected(true);
		}
		else if (eventType & EVE_UnselectCurrentEntity)
		{
			GUINodeSelection* key = Internal::KeyFromValue(m_nodeToEntityMap, m_selectedEntity);
			key->SetSelected(false);
			m_selectedEntity = nullptr;
		}
		else if (eventType & EVE_PreSavingCurrentLevel)
		{
			OStream& outStream = m_levelManager->GetCurrentLevel()->GetOutRuntimeUserStream();

			Vector<GUINodeSelection*> items;
			items.reserve(m_layout->GetChildren().size());

			for (auto c : m_layout->GetChildren())
				items.push_back(static_cast<GUINodeSelection*>(c));

			SaveHierarchy(outStream, items);
		}
	}

	void GUINodePanelEntities::OnPayloadAccepted()
	{
		Entity* src			 = static_cast<Entity*>(m_editor->GetPayloadManager().GetCurrentPayloadMeta().data);
		auto*	srcSelection = Internal::KeyFromValue(m_nodeToEntityMap, src);

		// Remove the selection node of the source entity from its parent.
		{
			if (src->GetParent() != nullptr)
			{
				auto* srcParent			 = src->GetParent();
				auto* srcParentSelection = Internal::KeyFromValue(m_nodeToEntityMap, srcParent);
				srcParentSelection->RemoveChildSelection(srcSelection);
				src->RemoveFromParent();
				m_layout->AddChildren(srcSelection);
				srcSelection->SetXOffset(0);
			}
		}
	}

	bool GUINodePanelEntities::OnMouse(uint32 button, LinaGX::InputAction action)
	{
		const bool retVal = GUINode::OnMouse(button, action);

		if (action == LinaGX::InputAction::Released)
		{
			if (button == LINAGX_MOUSE_1)
			{
				for (auto [node, entity] : m_nodeToEntityMap)
				{
					if (node->GetIsHovered())
					{
						m_editor->GetCommandManager()->AddCommand(new EditorCommandSelectEntity(m_editor, m_selectedEntity, entity));
						m_contextMenuSelection->SetPos(m_window->GetMousePosition());
						m_contextMenuSelection->SetVisible(true);
						m_contextMenuBoldToggle->SetValue(node->GetIsBoldFont());
						return retVal;
					}
				}
			}

			m_contextMenuSelection->SetVisible(false);
		}

		return retVal;
	}

	void GUINodePanelEntities::RecreateEntityHierarchy()
	{
		ClearEntityHierarchy();

		Vector<Entity*> entities;
		m_world->GetAllEntities(entities);

		Vector<Entity*> parentlessEntities;
		parentlessEntities.reserve(entities.size());

		for (auto e : entities)
		{
			if (e->GetParent() == nullptr)
				parentlessEntities.push_back(e);
		}

		CreateHierarchyForList(parentlessEntities, nullptr);
	}

	void GUINodePanelEntities::CreateHierarchyForList(const Vector<Entity*>& list, GUINodeSelection* parent)
	{
		for (auto* entity : list)
		{
			GUINodeSelection* selection = CreateSelectionForEntity(entity);

			if (parent)
				parent->AddChildSelection(selection);
			else
				m_layout->AddChildren(selection);

			const Vector<Entity*>& children = entity->GetChildren();
			CreateHierarchyForList(children, selection);
		}
	}

	void GUINodePanelEntities::ClearEntityHierarchy()
	{
		m_nodeToEntityMap.clear();
		RemoveChildren(m_layout);
		delete m_layout;
		m_layout = new GUINodeLayoutVertical(m_drawer, m_drawOrder);
		m_layout->SetFixedHorizontalSize(true);
		AddChildren(m_layout);
	}

	void GUINodePanelEntities::LoadHierarchy(IStream& inStream, GUINodeSelection* parent)
	{
		uint32 sz = 0;
		inStream >> sz;

		for (uint32 i = 0; i < sz; i++)
		{
			StringID sid		 = 0;
			bool	 expandState = false;
			inStream >> sid >> expandState;

			Entity* e = m_world->GetEntityFromID(sid);

			GUINodeSelection* selection = CreateSelectionForEntity(e);
			selection->SetIsExpanded(expandState);

			if (parent == nullptr)
				m_layout->AddChildren(selection);
			else
				parent->AddChildSelection(selection);

			LoadHierarchy(inStream, selection);
		}
	}

	void GUINodePanelEntities::SaveHierarchy(OStream& outStream, const Vector<GUINodeSelection*>& items)
	{
		const uint32 sz = static_cast<uint32>(items.size());
		outStream << sz;

		for (uint32 i = 0; i < sz; i++)
		{
			auto*		   item		   = items[i];
			const StringID sid		   = item->GetSID();
			const bool	   expandState = item->GetIsExpanded();
			outStream << sid;
			outStream << expandState;

			const Vector<GUINodeSelection*>& children = item->GetSelectionChildren();
			SaveHierarchy(outStream, children);
		}
	}

	void GUINodePanelEntities::OnClickedSelection(GUINode* selectionNode)
	{
		auto	sel	   = static_cast<GUINodeSelection*>(selectionNode);
		Entity* target = m_nodeToEntityMap[sel];
		m_editor->GetCommandManager()->AddCommand(new EditorCommandSelectEntity(m_editor, m_selectedEntity, target));
	}

	void GUINodePanelEntities::OnSelectionDetached(GUINodeSelection* selection, const Vector2& delta)
	{
		const float padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		m_editor->GetPayloadManager().CreatePayload(
			PayloadType::EPL_Entity, selection->GetStoreSize("TitleSize"_hs, selection->GetTitle()) + Vector2(padding * 2, padding), Vector2i(static_cast<int32>(delta.x), static_cast<int32>(delta.y)), m_nodeToEntityMap[selection]);
	}

	void GUINodePanelEntities::OnSelectionPayloadAccepted(GUINode* selection, void* userData)
	{
		auto	sel			  = static_cast<GUINodeSelection*>(selection);
		Entity* target		  = m_nodeToEntityMap[sel];
		Entity* draggedEntity = static_cast<Entity*>(m_editor->GetPayloadManager().GetCurrentPayloadMeta().data);

		if (draggedEntity == target)
			return;

		m_entityDropData.active		  = true;
		m_entityDropData.srcEntity	  = draggedEntity;
		m_entityDropData.targetEntity = target;
		m_entityDropData.dropState	  = *static_cast<int*>(userData);
	}

	GUINodeSelection* GUINodePanelEntities::CreateSelectionForEntity(Entity* e)
	{
		GUINodeSelection* selection = new GUINodeSelection(m_drawer, m_drawOrder);
		selection->SetCallbackClicked(BIND(&GUINodePanelEntities::OnClickedSelection, this, std::placeholders::_1));
		selection->SetCallbackDetached(BIND(&GUINodePanelEntities::OnSelectionDetached, this, std::placeholders::_1, std::placeholders::_2));
		selection->SetCallbackPayloadAccepted(BIND(&GUINodePanelEntities::OnSelectionPayloadAccepted, this, std::placeholders::_1, std::placeholders::_2));
		selection->SetTitle(e->GetName().c_str());
		selection->GetPayloadMask().Set(EPL_Entity);
		selection->SetSID(e->GetID());
		m_nodeToEntityMap[selection] = e;
		return selection;
	}

	void GUINodePanelEntities::LoadLevelMeta()
	{
		auto& loadedStream = m_levelManager->GetCurrentLevel()->GetLoadedRuntimeUserStream();

		if (loadedStream.GetSize() != 0)
		{
			ClearEntityHierarchy();
			LoadHierarchy(loadedStream, nullptr);
		}
		else
			RecreateEntityHierarchy();
	}

	void GUINodePanelEntities::OnPressedContextItem(GUINode* node)
	{
		const StringID sid		 = node->GetSID();
		auto*		   selection = Internal::KeyFromValue(m_nodeToEntityMap, m_selectedEntity);

		if (sid == "Bold Font"_hs)
			selection->SetIsBoldFont(!selection->GetIsBoldFont());
		else if (sid == "None"_hs)
			selection->SetIsHighlightEnabled(false);
		else if (sid == "Select"_hs)
		{
		}
	}
} // namespace Lina::Editor
