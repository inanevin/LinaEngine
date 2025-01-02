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
#include "Editor/Graphics/EditorWorldRenderer.hpp"
#include "Editor/Widgets/World/WorldController.hpp"
#include "Common/Math/Math.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Application.hpp"

namespace Lina::Editor
{
	void WorldDisplayer::Construct()
	{
		m_editor = Editor::Get();

		GetWidgetProps().outlineThickness = 0.0f;
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

		m_worldController = m_manager->Allocate<WorldController>("WorldController");
		m_worldController->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_worldController->SetAlignedPos(Vector2::Zero);
		m_worldController->SetAlignedSize(Vector2::One);
		AddChild(m_worldController);
	}

	void WorldDisplayer::Initialize()
	{
		m_noWorldText->UpdateTextAndCalcSize(m_props.noWorldText);
	}

	void WorldDisplayer::DisplayWorld(EditorWorldRenderer* ewr, WorldCameraType cameraType)
	{
		m_ewr = ewr;
		m_wr  = ewr != nullptr ? ewr->GetWorldRenderer() : nullptr;
		m_noWorldText->GetFlags().Set(WF_HIDE, m_wr != nullptr);
		GetWidgetProps().lvgUserData	 = m_wr == nullptr ? nullptr : &m_guiUserData;
		GetWidgetProps().colorBackground = Color::White;
		m_worldController->SetWorld(m_wr, m_ewr, cameraType);
	}

	void WorldDisplayer::PreTick()
	{
		if (m_wr == nullptr)
			return;

		// Screen setup
		Screen& sc = m_wr->GetWorld()->GetScreen();
		sc.SetOwnerWindow(m_lgxWindow);
		sc.SetDisplaySize(m_rect.size);
		sc.SetDisplayPos(m_rect.pos);

		bool skipSize = false;
		if (m_rect.size.x < 0.0f || m_rect.size.y < 0.0f)
			skipSize = true;

		const Vector2ui displayerSize = Vector2ui(Math::FloorToInt(m_rect.size.x), Math::FloorToInt(m_rect.size.y));
		if (!skipSize && (sc.GetRenderSize().x != displayerSize.x || sc.GetRenderSize().y != displayerSize.y))
		{
			m_editor->GetApp()->JoinRender();
			m_wr->Resize(displayerSize);
			sc.SetRenderSize(displayerSize);
		}
	}

	void WorldDisplayer::Tick(float dt)
	{
		if (m_wr == nullptr)
			return;

		const uint32 frameIndex	   = Application::GetLGX()->GetCurrentFrameIndex();
		const uint32 txtFrameIndex = (frameIndex + SystemInfo::GetRendererBehindFrames()) % FRAMES_IN_FLIGHT;
		Texture*	 target		   = nullptr;

		const WorldController::DisplayTexture dp = m_worldController->GetCurrentDisplayTexture();
		if (dp == WorldController::DisplayTexture::WorldFinal)
			target = m_ewr->GetRenderTarget(txtFrameIndex);
		else if (dp == WorldController::DisplayTexture::WorldGBuf0)
			target = m_wr->GetGBufAlbedo(txtFrameIndex);
		else if (dp == WorldController::DisplayTexture::WorldGBuf1)
			target = m_wr->GetGBufPosition(txtFrameIndex);
		else if (dp == WorldController::DisplayTexture::WorldGBuf2)
			target = m_wr->GetGBufNormal(txtFrameIndex);
		else if (dp == WorldController::DisplayTexture::WorldDepth)
			target = m_wr->GetGBufDepth(txtFrameIndex);
		else if (dp == WorldController::DisplayTexture::WorldResult)
			target = m_wr->GetLightingPassOutput(txtFrameIndex);
		else if (dp == WorldController::DisplayTexture::OutlinePass)
			target = m_ewr->GetOutlineRenderer().GetRenderTarget(txtFrameIndex);

		GetWidgetProps().rawTexture = target;
	}

} // namespace Lina::Editor
