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

#include "Editor/Widgets/World/WorldDisplayer.hpp"
#include "Editor/Editor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Widgets/Panel/PanelResourceBrowser.hpp"
#include "Editor/Widgets/Compound/ResourceDirectoryBrowser.hpp"
#include "Editor/Graphics/EditorWorldRenderer.hpp"
#include "Editor/World/EditorCamera.hpp"
#include "Editor/World/WorldUtility.hpp"
#include "Editor/Actions/EditorActionEntity.hpp"

#include "Core/GUI/Widgets/Layout/Popup.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Application.hpp"
#include "Common/Math/Math.hpp"
#include "Common/System/SystemInfo.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
	void WorldDisplayer::Construct()
	{
		m_editor = Editor::Get();

		GetFlags().Set(WF_KEY_PASSTHRU | WF_MOUSE_PASSTHRU);
		GetWidgetProps().outlineThickness = Theme::GetDef().baseOutlineThickness;
		GetWidgetProps().rounding		  = 0.0f;
		GetWidgetProps().drawBackground	  = true;
		GetWidgetProps().colorBackground  = Theme::GetDef().background0;
		GetWidgetProps().childMargins	  = TBLR::Eq(Theme::GetDef().baseIndent);

		m_noWorldText = m_manager->Allocate<Text>("NoWorld");
		m_noWorldText->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		m_noWorldText->SetAlignedPos(Vector2(0.5f, 0.5f));
		m_noWorldText->SetAnchorX(Anchor::Center);
		m_noWorldText->SetAnchorY(Anchor::Center);
		m_noWorldText->GetProps().font	= Theme::GetDef().altBigFont;
		m_noWorldText->GetProps().color = Theme::GetDef().silent2;
		AddChild(m_noWorldText);

		DirectionalLayout* topToolbar = m_manager->Allocate<DirectionalLayout>("TopToolbar");
		topToolbar->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_X_TOTAL_CHILDREN | WF_USE_FIXED_SIZE_Y);
		topToolbar->SetAlignedPos(Vector2::Zero);
		topToolbar->SetAlignedSizeX(1.0f);
		topToolbar->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		topToolbar->SetFixedSizeX(Theme::GetDef().baseItemHeight);
		topToolbar->GetWidgetProps().childPadding		= Theme::GetDef().baseIndent;
		topToolbar->GetWidgetProps().drawOrderIncrement = 1;

		AddChild(topToolbar);

		m_displayTextureDropdown = m_manager->Allocate<Dropdown>("DisplayTextureDD");
		m_displayTextureDropdown->GetFlags().Set(WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_SIZE_ALIGN_Y);
		m_displayTextureDropdown->SetAlignedPosY(0.0f);
		m_displayTextureDropdown->SetAlignedSizeY(1.0f);
		m_displayTextureDropdown->SetFixedSizeX(Theme::GetDef().baseItemHeight * 8);
		topToolbar->AddChild(m_displayTextureDropdown);

		SetDisplayTextureTitle();
		m_displayTextureDropdown->GetProps().onAddItems = [this](Popup* popup) {
			popup->AddToggleItem("Final Composition", m_currentDisplayTexture == DisplayTexture::WorldFinal, 0);
			popup->AddToggleItem("GBuffer 0", m_currentDisplayTexture == DisplayTexture::WorldGBuf0, 1);
			popup->AddToggleItem("GBuffer 1", m_currentDisplayTexture == DisplayTexture::WorldGBuf1, 2);
			popup->AddToggleItem("GBuffer 2", m_currentDisplayTexture == DisplayTexture::WorldGBuf2, 3);
			popup->AddToggleItem("GBuffer 3", m_currentDisplayTexture == DisplayTexture::WorldDepth, 4);
			popup->AddToggleItem("World Output", m_currentDisplayTexture == DisplayTexture::WorldResult, 5);
			popup->AddToggleItem("Editor Outline", m_currentDisplayTexture == DisplayTexture::OutlinePass, 6);
		};

		m_displayTextureDropdown->GetProps().onSelected = [this](int32 idx, String& outTitle) -> bool {
			m_currentDisplayTexture = static_cast<DisplayTexture>(idx);
			SetDisplayTextureTitle();
			return false;
		};

		m_gizmoFont = Editor::Get()->GetApp()->GetResourceManager().GetResource<Font>(EDITOR_FONT_PLAY_ID);
		m_editor->GetWindowPanelManager().AddPayloadListener(this);

		m_gizmoControls.selectedGizmo = static_cast<GizmoType>(m_editor->GetSettings().GetParams().GetParamUint8("GizmoType"_hs, 0));
		m_gizmoControls.gizmoLocality = static_cast<GizmoLocality>(m_editor->GetSettings().GetParams().GetParamUint8("GizmoLocality"_hs, 0));
		m_gizmoControls.gizmoSnapping = static_cast<GizmoSnapping>(m_editor->GetSettings().GetParams().GetParamUint8("GizmoSnapping"_hs, 0));
	}

	void WorldDisplayer::Initialize()
	{
		m_noWorldText->UpdateTextAndCalcSize(m_props.noWorldText);
	}

	void WorldDisplayer::Destruct()
	{
		m_editor->GetWindowPanelManager().RemovePayloadListener(this);

		if (m_worldRenderer)
			m_worldRenderer->GetWorld()->RemoveListener(this);

		DestroyCamera();
	}

	void WorldDisplayer::DisplayWorld(WorldRenderer* renderer, EditorWorldRenderer* ewr, WorldCameraType cameraType)
	{
		DestroyCamera();
		m_noWorldText->GetFlags().Set(WF_HIDE, renderer != nullptr);

		if (renderer == nullptr)
			return;

		m_worldRenderer = renderer;
		m_worldRenderer->GetWorld()->AddListener(this);
		m_ewr						 = ewr;
		GetWidgetProps().lvgUserData = renderer == nullptr ? nullptr : &m_guiUserData;

		if (cameraType == WorldCameraType::Orbit)
			m_camera = new OrbitCamera(m_worldRenderer->GetWorld());
		else if (cameraType == WorldCameraType::FreeMove)
			m_camera = new FreeCamera(m_worldRenderer->GetWorld());

		GetWidgetProps().colorBackground = Color::White;
	}

	void WorldDisplayer::PreTick()
	{
		if (m_worldRenderer == nullptr)
			return;

		// Input setup
		const bool worldHasFocus = m_manager->GetControlsOwner() == this && m_lgxWindow->HasFocus();
		m_worldRenderer->GetWorld()->GetInput().SetIsActive(worldHasFocus);
		m_worldRenderer->GetWorld()->GetInput().SetWheelActive(m_isHovered && m_lgxWindow->HasFocus());

		// Screen setup
		Screen& sc = m_worldRenderer->GetWorld()->GetScreen();
		sc.SetOwnerWindow(m_lgxWindow);
		sc.SetDisplaySize(m_rect.size);
		sc.SetDisplayPos(m_rect.pos);

		bool skipSize = false;
		if (m_rect.size.x < 0.0f || m_rect.size.y < 0.0f)
			skipSize = true;

		const Vector2ui displayerSize = Vector2ui(Math::FloorToInt(m_rect.size.x), Math::FloorToInt(m_rect.size.y));
		if (!skipSize && (sc.GetRenderSize().x != displayerSize.x || sc.GetRenderSize().y != displayerSize.y))
		{
			Editor::Get()->GetApp()->JoinRender();
			m_worldRenderer->Resize(displayerSize);
			sc.SetRenderSize(displayerSize);
		}

		HandleGizmoControls();
	}

	void WorldDisplayer::Tick(float dt)
	{
		if (m_worldRenderer == nullptr)
			return;

		const uint32 frameIndex	   = Application::GetLGX()->GetCurrentFrameIndex();
		const uint32 txtFrameIndex = (frameIndex + SystemInfo::GetRendererBehindFrames()) % 2;
		Texture*	 target		   = nullptr;

		if (m_currentDisplayTexture == DisplayTexture::WorldFinal)
			target = m_ewr->GetRenderTarget(txtFrameIndex);
		else if (m_currentDisplayTexture == DisplayTexture::WorldGBuf0)
			target = m_worldRenderer->GetGBufAlbedo(txtFrameIndex);
		else if (m_currentDisplayTexture == DisplayTexture::WorldGBuf1)
			target = m_worldRenderer->GetGBufPosition(txtFrameIndex);
		else if (m_currentDisplayTexture == DisplayTexture::WorldGBuf2)
			target = m_worldRenderer->GetGBufNormal(txtFrameIndex);
		else if (m_currentDisplayTexture == DisplayTexture::WorldDepth)
			target = m_worldRenderer->GetGBufDepth(txtFrameIndex);
		else if (m_currentDisplayTexture == DisplayTexture::WorldResult)
			target = m_worldRenderer->GetLightingPassOutput(txtFrameIndex);
		else if (m_currentDisplayTexture == DisplayTexture::OutlinePass)
			target = m_ewr->GetOutlineRenderer().GetRenderTarget(txtFrameIndex);

		GetWidgetProps().rawTexture = target;
	}

	void WorldDisplayer::Draw()
	{
		if (m_worldRenderer == nullptr)
			return;

		DrawAxis(Vector3::Forward, Theme::GetDef().accentSecondary, "Z");
		DrawAxis(Vector3::Up, Theme::GetDef().accentSuccess, "Y");
		DrawAxis(Vector3::Right, Theme::GetDef().accentError, "X");
	}

	void WorldDisplayer::OnPayloadStarted(PayloadType type, Widget* payload)
	{
		if (!m_props.enableDragAndDrop || type != PayloadType::Resource || m_worldRenderer == nullptr)
			return;
	}

	void WorldDisplayer::OnPayloadEnded(PayloadType type, Widget* payload)
	{
		if (!m_props.enableDragAndDrop || type != PayloadType::Resource || m_worldRenderer == nullptr)
			return;
	}

	bool WorldDisplayer::OnPayloadDropped(PayloadType type, Widget* payload)
	{
		if (!m_props.enableDragAndDrop || type != PayloadType::Resource || m_worldRenderer == nullptr)
			return false;
		if (!m_isHovered)
			return false;

		Panel* panel = m_editor->GetWindowPanelManager().FindPanelOfType(PanelType::ResourceBrowser, 0);

		if (panel == nullptr)
			return false;

		const Vector<ResourceDirectory*>& payloadItems = static_cast<PanelResourceBrowser*>(panel)->GetBrowser()->GetPayloadItems();

		HashSet<ResourceID> resources;

		Entity* last = nullptr;

		EntityWorld* world = m_worldRenderer->GetWorld();

		for (ResourceDirectory* dir : payloadItems)
		{
			if (dir->resourceTID == GetTypeID<Model>())
			{
				WorldUtility::LoadModelAndMaterials(m_editor, dir->resourceID, world->GetID());

				Model*	model  = m_resourceManager->GetResource<Model>(dir->resourceID);
				Entity* entity = WorldUtility::AddModelToWorld(world, model, model->GetMeta().materials);
				world->LoadMissingResources(*m_resourceManager, m_editor->GetProjectManager().GetProjectData(), {}, world->GetID());

				const Vector2 mp	= m_lgxWindow->GetMousePosition() - GetStartFromMargins();
				const Vector2 size	= GetEndFromMargins() - GetStartFromMargins();
				const Vector3 point = Camera::ScreenToWorld(world->GetWorldCamera(), mp, size, 0.97f);
				entity->SetPosition(point);

				last = entity;
			}
		}

		SelectEntity(last, true);
		return true;
	}

	bool WorldDisplayer::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (m_isHovered)
			m_manager->GrabControls(this);
		else
			m_manager->ReleaseControls(this);

		if (m_worldRenderer == nullptr)
			return false;

		for (Widget* c : m_children)
		{
			if (c->OnMouse(button, act))
				return false;
		}

		if (m_isHovered && button == LINAGX_MOUSE_0 && (act == LinaGX::InputAction::Pressed))
		{
			const EntityID lastHovered = m_ewr->GetMousePick().GetLastHoveredEntity();

			if (lastHovered == GIZMO_GUID_X_AXIS)
			{
				m_gizmoControls.pressedAxis = GizmoAxis::X;
			}
			else if (lastHovered == GIZMO_GUID_Y_AXIS)
			{
				m_gizmoControls.pressedAxis = GizmoAxis::Y;
			}
			else if (lastHovered == GIZMO_GUID_Z_AXIS)
			{
				m_gizmoControls.pressedAxis = GizmoAxis::Z;
			}
			else
				SelectEntity(lastHovered, true);
		}

		return false;
	}

	bool WorldDisplayer::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act)
	{
		if (m_worldRenderer == nullptr)
			return false;

		if (m_manager->GetControlsOwner() != this)
			return false;

		if (keycode == LINAGX_KEY_1 && act == LinaGX::InputAction::Pressed)
		{
			SelectGizmo(GizmoType::Move);
			return true;
		}

		if (keycode == LINAGX_KEY_2 && act == LinaGX::InputAction::Pressed)
		{
			SelectGizmo(GizmoType::Rotate);
			return true;
		}

		if (keycode == LINAGX_KEY_3 && act == LinaGX::InputAction::Pressed)
		{
			SelectGizmo(GizmoType::Scale);
			return true;
		}

		if (keycode == LINAGX_KEY_4 && act == LinaGX::InputAction::Pressed)
		{
			if (m_gizmoControls.gizmoLocality == GizmoLocality::Local)
				SelectGizmoLocality(GizmoLocality::World);
			else
				SelectGizmoLocality(GizmoLocality::Local);

			return true;
		}

		if (keycode == LINAGX_KEY_5 && act == LinaGX::InputAction::Pressed)
		{
			const uint8 current = static_cast<uint8>(m_gizmoControls.gizmoSnapping);
			const uint8 target	= (current + 1) % 3;
			SelectGizmoSnap(static_cast<GizmoSnapping>(target));
			return true;
		}

		return true;
	}

	void WorldDisplayer::SelectEntity(Entity* e, bool clearOthers)
	{
		const Vector<Entity*> prev = m_selectedEntities;

		if (clearOthers)
			m_selectedEntities.clear();

		if (e != nullptr)
			m_selectedEntities.push_back(e);

		EditorActionEntitySelection::Create(m_editor, m_worldRenderer->GetWorld()->GetID(), prev, m_selectedEntities);

		OnEntitySelectionChanged();
	}

	void WorldDisplayer::SelectEntity(EntityID guid, bool clearOthers)
	{
		SelectEntity(m_worldRenderer->GetWorld()->GetEntity(guid), clearOthers);
	}

	void WorldDisplayer::OnActionEntitySelection(const Vector<EntityID>& selection)
	{
		m_selectedEntities.clear();
		for (EntityID guid : selection)
			m_selectedEntities.push_back(m_worldRenderer->GetWorld()->GetEntity(guid));
		OnEntitySelectionChanged();
	}

	void WorldDisplayer::SelectGizmo(GizmoType gizmo)
	{
		m_gizmoControls.selectedGizmo = gizmo;
		m_ewr->GetGizmoRenderer().SetSelectedGizmo(gizmo);
		m_editor->GetSettings().GetParams().SetParamUint8("GizmoType"_hs, static_cast<uint8>(gizmo));
		m_editor->SaveSettings();
	}

	void WorldDisplayer::SelectGizmoLocality(GizmoLocality locality)
	{
		// Can't on multiselect
		if (locality == GizmoLocality::Local && m_selectedEntities.size() > 1)
			locality = GizmoLocality::World;

		m_gizmoControls.gizmoLocality = locality;
		m_ewr->GetGizmoRenderer().SetGizmoLocality(locality);
		m_editor->GetSettings().GetParams().SetParamUint8("GizmoLocality"_hs, static_cast<uint8>(locality));
		m_editor->SaveSettings();
	}

	void WorldDisplayer::SelectGizmoSnap(GizmoSnapping snapping)
	{
		m_gizmoControls.gizmoSnapping = snapping;
		m_editor->GetSettings().GetParams().SetParamUint8("GizmoSnapping"_hs, static_cast<uint8>(snapping));
		m_editor->SaveSettings();
	}

	void WorldDisplayer::OnEntitySelectionChanged()
	{
		m_ewr->SetSelectedEntities(m_selectedEntities);

		if (m_selectedEntities.size() > 1)
		{
			SelectGizmoLocality(GizmoLocality::World);
		}
	}

	ResourceID WorldDisplayer::GetWorldID()
	{
		return m_worldRenderer == nullptr ? 0 : m_worldRenderer->GetWorld()->GetID();
	}

	void WorldDisplayer::SetDisplayTextureTitle()
	{
		if (m_currentDisplayTexture == DisplayTexture::WorldFinal)
			m_displayTextureDropdown->GetText()->UpdateTextAndCalcSize("Final Composition");
		else if (m_currentDisplayTexture == DisplayTexture::WorldGBuf0)
			m_displayTextureDropdown->GetText()->UpdateTextAndCalcSize("GBuffer 0");
		else if (m_currentDisplayTexture == DisplayTexture::WorldGBuf1)
			m_displayTextureDropdown->GetText()->UpdateTextAndCalcSize("GBuffer 1");
		else if (m_currentDisplayTexture == DisplayTexture::WorldGBuf2)
			m_displayTextureDropdown->GetText()->UpdateTextAndCalcSize("GBuffer 2");
		else if (m_currentDisplayTexture == DisplayTexture::WorldDepth)
			m_displayTextureDropdown->GetText()->UpdateTextAndCalcSize("GBuffer Depth");
		else if (m_currentDisplayTexture == DisplayTexture::WorldResult)
			m_displayTextureDropdown->GetText()->UpdateTextAndCalcSize("World Output");
		else if (m_currentDisplayTexture == DisplayTexture::OutlinePass)
			m_displayTextureDropdown->GetText()->UpdateTextAndCalcSize("Editor Outline");
	}

	void WorldDisplayer::HandleGizmoControls()
	{
		if (m_gizmoControls.pressedAxis != GizmoAxis::None && !m_lgxWindow->GetInput()->GetMouseButton(LINAGX_MOUSE_0))
			m_gizmoControls.pressedAxis = GizmoAxis::None;

		// Hovered state.
		{
			const EntityID hovered	   = m_ewr->GetMousePick().GetLastHoveredEntity();
			GizmoAxis	   hoveredAxis = GizmoAxis::None;

			if (m_gizmoControls.pressedAxis != GizmoAxis::None)
				hoveredAxis = m_gizmoControls.pressedAxis;
			else if (hovered == GIZMO_GUID_X_AXIS)
				hoveredAxis = GizmoAxis::X;
			else if (hovered == GIZMO_GUID_Y_AXIS)
				hoveredAxis = GizmoAxis::Y;
			else if (hovered == GIZMO_GUID_Z_AXIS)
				hoveredAxis = GizmoAxis::Z;
			else
				hoveredAxis == GizmoAxis::None;

			m_ewr->GetGizmoRenderer().SetHoveredAxis(hoveredAxis);
			m_gizmoControls.hoveredAxis = hoveredAxis;
		}

		m_ewr->GetGizmoRenderer().SetPressedAxis(m_gizmoControls.pressedAxis);
	}

	void WorldDisplayer::DestroyCamera()
	{
		if (m_camera == nullptr)
			return;
		delete m_camera;
		m_camera = nullptr;
	}

	void WorldDisplayer::OnWorldTick(float delta, PlayMode playmode)
	{
		if (m_camera)
			m_camera->Tick(delta);
	}

	void WorldDisplayer::DrawAxis(const Vector3& targetAxis, const Color& baseColor, const String& axis)
	{
		EntityWorld*  world	   = m_worldRenderer->GetWorld();
		const Camera& worldCam = world->GetWorldCamera();

		const float length		 = 40;
		const float circleRadius = 10.0f;
		const int	lineDO		 = m_drawOrder + 1;
		const int	circleDO	 = lineDO + 1;
		const int	textDO		 = circleDO + 1;

		const Vector2 displaySize	 = world->GetScreen().GetDisplaySize();
		const Vector2 axesGizmoStart = Vector2(m_rect.pos.x + Theme::GetDef().baseIndent * 6, m_rect.GetEnd().y - Theme::GetDef().baseIndent * 6);
		const Vector3 front			 = worldCam.GetPosition() + worldCam.GetRotation().GetForward() * 1.0f;

		const Vector3 target   = targetAxis * worldCam.GetZFar();
		const float	  dot	   = Math::Abs(worldCam.GetRotation().GetForward().Dot(targetAxis));
		const Vector2 screen0  = Camera::WorldToScreen(worldCam, front, displaySize).XY();
		const Vector2 screen1  = Camera::WorldToScreen(worldCam, target, displaySize).XY();
		const Vector2 dir	   = (screen1 - screen0).Normalized();
		const Vector2 line0	   = axesGizmoStart;
		const Vector2 line1	   = line0 + dir * (1.0f - dot) * length;
		const Vector2 line1Neg = line0 - dir * (1.0f - dot) * length;

		const float overallAlpha = Math::Remap((line1 - line0).Magnitude(), 0.0f, length, 0.1f, 1.0f);

		LinaVG::StyleOptions lineStyle;
		lineStyle.thickness = Theme::GetDef().baseOutlineThickness;
		lineStyle.aaEnabled = true;
		lineStyle.color		= baseColor.AsLVG4();

		LinaVG::StyleOptions circleStyle;
		circleStyle.thickness					 = Theme::GetDef().baseOutlineThickness;
		circleStyle.outlineOptions.thickness	 = Theme::GetDef().baseOutlineThickness;
		circleStyle.outlineOptions.drawDirection = LinaVG::OutlineDrawDirection::Inwards;
		circleStyle.outlineOptions.color		 = baseColor.AsLVG4();
		circleStyle.color						 = baseColor.AsLVG4();
		circleStyle.color.start.w = circleStyle.color.end.w = 0.5f * overallAlpha;
		circleStyle.outlineOptions.color.start.w = circleStyle.outlineOptions.color.end.w = overallAlpha;

		LinaVG::TextOptions gizmoText;
		gizmoText.font			= m_gizmoFont->GetFont(m_lgxWindow->GetDPIScale());
		gizmoText.color			= Theme::GetDef().foreground0.AsLVG4();
		gizmoText.color.start.w = gizmoText.color.end.w = overallAlpha;

		m_lvg->DrawLine(line0.AsLVG(), line1.AsLVG(), lineStyle, LinaVG::LineCapDirection::None, 0.0f, lineDO);
		m_lvg->DrawCircle(line1.AsLVG(), circleRadius, circleStyle, 36, 0.0f, 0.0f, 360.0f, circleDO);

		m_lvg->DrawCircle(line1Neg.AsLVG(), circleRadius, circleStyle, 36, 0.0f, 0.0f, 360.0f, circleDO);

		const Vector2 sz = m_lvg->CalculateTextSize(axis.c_str(), gizmoText);
		m_lvg->DrawTextDefault(axis.c_str(), (line1 + Vector2(-sz.x * 0.5f, sz.y * 0.4f)).AsLVG(), gizmoText, 0.0f, textDO, false);
	}

} // namespace Lina::Editor
