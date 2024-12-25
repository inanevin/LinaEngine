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
#include "Editor/Graphics/EditorWorldRenderer.hpp"
#include "Editor/World/EditorCamera.hpp"
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
		GetFlags().Set(WF_KEY_PASSTHRU | WF_MOUSE_PASSTHRU);
		GetWidgetProps().outlineThickness = Theme::GetDef().baseOutlineThickness;
		GetWidgetProps().rounding		  = 0.0f;
		GetWidgetProps().drawBackground	  = true;
		GetWidgetProps().colorBackground  = Theme::GetDef().background0;

		m_noWorldText = m_manager->Allocate<Text>("NoWorld");
		m_noWorldText->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		m_noWorldText->SetAlignedPos(Vector2(0.5f, 0.5f));
		m_noWorldText->SetAnchorX(Anchor::Center);
		m_noWorldText->SetAnchorY(Anchor::Center);
		m_noWorldText->GetProps().font	= Theme::GetDef().altBigFont;
		m_noWorldText->GetProps().color = Theme::GetDef().silent2;
		AddChild(m_noWorldText);

		m_gizmoFont = Editor::Get()->GetApp()->GetResourceManager().GetResource<Font>(EDITOR_FONT_PLAY_ID);
	}

	void WorldDisplayer::Initialize()
	{
		m_noWorldText->UpdateTextAndCalcSize(m_props.noWorldText);
	}

	void WorldDisplayer::Destruct()
	{
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
		m_ewr = ewr;

		if (cameraType == WorldCameraType::Orbit)
			m_camera = new OrbitCamera(m_worldRenderer->GetWorld());
		else if (cameraType == WorldCameraType::FreeMove)
			m_camera = new FreeCamera(m_worldRenderer->GetWorld());

		GetWidgetProps().colorBackground = Color::White;
	}

	void WorldDisplayer::PreTick()
	{
		if (m_mouseConfined && !m_lgxWindow->GetInput()->GetMouseButton(LINAGX_MOUSE_1))
		{
			// m_lgxWindow->FreeMouse();
			// m_lgxWindow->SetMouseVisible(true);
			m_mouseConfined = false;
		}

		if (m_worldRenderer == nullptr)
			return;

		// Input setup
		const bool worldHasFocus = m_manager->GetControlsOwner() == this && m_lgxWindow->HasFocus();
		m_worldRenderer->GetWorld()->GetInput().SetIsActive(worldHasFocus);
		m_worldRenderer->GetWorld()->GetInput().SetWheelActive(m_isHovered);

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
	}

	void WorldDisplayer::Tick(float dt)
	{
		if (m_worldRenderer == nullptr)
			return;

		const uint32 frameIndex		= Application::GetLGX()->GetCurrentFrameIndex();
		const uint32 txtFrameIndex	= (frameIndex + SystemInfo::GetRendererBehindFrames()) % 2;
		Texture*	 target			= m_ewr->GetRenderTarget(txtFrameIndex); // 1 frame behind renderer
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

	bool WorldDisplayer::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (m_isHovered && button == LINAGX_MOUSE_1 && (act == LinaGX::InputAction::Pressed || act == LinaGX::InputAction::Repeated))
		{
			m_manager->GrabControls(this);

			// const LinaGX::LGXVector2ui center = {static_cast<uint32>(m_lgxWindow->GetMousePosition().x), // static_cast<uint32>(m_lgxWindow->GetMousePosition().y)};
			// m_lgxWindow->ConfineMouseToPoint(center);
			// m_lgxWindow->SetMouseVisible(false);
			m_mouseConfined = true;
			return true;
		};

		if (m_isHovered)
		{
			m_manager->GrabControls(this);
			return true;
		}
		else
		{
			m_manager->ReleaseControls(this);
		}

		return false;
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
