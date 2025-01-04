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

#include "Editor/Widgets/Compound/EntityDetails.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Actions/EditorActionEntity.hpp"
#include "Common/Math/Math.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/World/Entity.hpp"
#include "Core/World/EntityWorld.hpp"

namespace Lina::Editor
{
	void EntityDetails::Construct()
	{
		m_editor = Editor::Get();

		m_layout = m_manager->Allocate<DirectionalLayout>("Vertical");
		m_layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_layout->SetAlignedPos(Vector2::Zero);
		m_layout->SetAlignedSize(Vector2::One);
		m_layout->GetProps().direction				   = DirectionOrientation::Vertical;
		m_layout->GetWidgetProps().childPadding		   = Theme::GetDef().baseIndent;
		m_layout->GetWidgetProps().childMargins.top	   = Theme::GetDef().baseIndent;
		m_layout->GetWidgetProps().childMargins.bottom = Theme::GetDef().baseIndent;
		AddChild(m_layout);

		m_noDetailsText = m_manager->Allocate<Text>("NoEntityDetails");
		m_noDetailsText->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		m_noDetailsText->SetAlignedPos(Vector2(0.5f, 0.5f));
		m_noDetailsText->SetAnchorX(Anchor::Center);
		m_noDetailsText->SetAnchorY(Anchor::Center);
		m_noDetailsText->GetProps().font = Theme::GetDef().altFont;
		m_noDetailsText->UpdateTextAndCalcSize(Locale::GetStr(LocaleStr::NoEntityDetails));
		AddChild(m_noDetailsText);
	}

	void EntityDetails::PreTick()
	{
		if (m_selectedEntities.size() != 1)
			return;

		UpdateDetails();
	}

	void EntityDetails::SetWorld(EntityWorld* w)
	{
		m_world = w;
		RefreshDetails();
	}

	void EntityDetails::OnEntitySelectionChanged(const Vector<Entity*>& entities)
	{
		m_selectedEntities = entities;
		RefreshDetails();
	}

	void EntityDetails::RefreshDetails()
	{
		m_noDetailsText->GetFlags().Set(WF_HIDE, !m_selectedEntities.empty());

		m_layout->DeallocAllChildren();
		m_layout->RemoveAllChildren();

		if (m_selectedEntities.empty())
			return;

		UpdateDetails();

		Widget* nameLayout = CommonWidgets::BuildStringLayout(
			this, Locale::GetStr(LocaleStr::Name), &m_dummyDetails.name, 0, [this]() { StartEditingName(); }, NULL, [this]() { StopEditingName(); });
		Widget* positionLayout = CommonWidgets::BuildVector3FLayout(
			this, Locale::GetStr(LocaleStr::Position), 0.1f, (uint8*)&m_dummyDetails.pos, 0, [this]() { StartEditingTransform(); }, [this]() { UpdateEntities(); }, [this]() { StopEditingTransform(); });
		Widget* rotationLayout = CommonWidgets::BuildVector3FLayout(
			this, Locale::GetStr(LocaleStr::Rotation), 0.1f, (uint8*)&m_dummyDetails.rot, 0, [this]() { StartEditingTransform(); }, [this]() { UpdateEntities(); }, [this]() { StopEditingTransform(); });
		Widget* scaleLayout = CommonWidgets::BuildVector3FLayout(
			this, Locale::GetStr(LocaleStr::Scale), 0.1f, (uint8*)&m_dummyDetails.scale, 0, [this]() { StartEditingTransform(); }, [this]() { UpdateEntities(); }, [this]() { StopEditingTransform(); });
		FoldLayout* physicsSettings = static_cast<FoldLayout*>(CommonWidgets::BuildFieldLayout(this, 0, Locale::GetStr(LocaleStr::Physics), true, &m_physicsSettingsFold));

		m_layout->AddChild(nameLayout);
		m_layout->AddChild(positionLayout);
		m_layout->AddChild(rotationLayout);
		m_layout->AddChild(scaleLayout);
		m_layout->AddChild(physicsSettings);
	}

	void EntityDetails::UpdateDetails()
	{
		if (m_isEditing)
			return;

		Entity* e			 = m_selectedEntities.at(0);
		m_dummyDetails.pos	 = e->GetLocalPosition();
		m_dummyDetails.rot	 = e->GetLocalRotation().GetPitchYawRoll();
		m_dummyDetails.scale = e->GetLocalScale();
		m_dummyDetails.name	 = e->GetName();
	}

	void EntityDetails::UpdateEntities()
	{
		if (Math::Equals(m_dummyDetails.scale.x, 0.0f, 0.001f))
			m_dummyDetails.scale.x = 0.001f;

		if (Math::Equals(m_dummyDetails.scale.y, 0.0f, 0.001f))
			m_dummyDetails.scale.y = 0.001f;

		if (Math::Equals(m_dummyDetails.scale.z, 0.0f, 0.001f))
			m_dummyDetails.scale.z = 0.001f;

		for (Entity* e : m_selectedEntities)
		{
			e->SetLocalPosition(m_dummyDetails.pos);
			e->SetLocalRotationAngles(m_dummyDetails.rot);
			e->SetLocalScale(m_dummyDetails.scale);
		}
	}
	void EntityDetails::StartEditingTransform()
	{
		m_isEditing = true;

		m_storedTransformations.resize(0);
		for (Entity* e : m_selectedEntities)
			m_storedTransformations.push_back(e->GetTransform());
	}

	void EntityDetails::StopEditingTransform()
	{
		m_isEditing = false;
		EditorActionEntityTransform::Create(m_editor, m_world->GetID(), m_selectedEntities, m_storedTransformations);
	}

	void EntityDetails::StartEditingName()
	{
		m_storedNames.resize(0);
		for (Entity* e : m_selectedEntities)
			m_storedNames.push_back(e->GetName());
	}

	void EntityDetails::StopEditingName()
	{
		for (Entity* e : m_selectedEntities)
			e->SetName(m_dummyDetails.name);
		EditorActionEntityNames::Create(m_editor, m_world->GetID(), m_selectedEntities, m_storedNames);
	}
} // namespace Lina::Editor
