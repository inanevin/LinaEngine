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

#include "Editor/Widgets/Screens/SplashScreen.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/Editor.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Common/System/System.hpp"
#include "Common/Math/Math.hpp"

namespace Lina::Editor
{
	void SplashScreen::Construct()
	{
		m_resourceManager->AddListener(this);

		m_versionText				   = m_manager->Allocate<Text>();
		m_versionText->GetProps().text = "v." + TO_STRING(LINA_VERSION_MAJOR) + "." + TO_STRING(LINA_VERSION_MINOR) + "." + TO_STRING(LINA_VERSION_PATCH);
		AddChild(m_versionText);

		m_infoText1					 = m_manager->Allocate<Text>();
		m_infoText1->GetProps().text = "Inan Evin [2018-] (c)";
		AddChild(m_infoText1);

		m_infoText2					 = m_manager->Allocate<Text>();
		m_infoText2->GetProps().text = "https://github.com/inanevin/LinaEngine";
		AddChild(m_infoText2);

		m_loadingInfo = m_manager->Allocate<Text>();
		AddChild(m_loadingInfo);

		m_splashImage	  = m_resourceManager->GetResource<Texture>("Resources/Editor/Textures/LinaLogoTitle.png"_hs);
		m_logoTextureSize = m_splashImage->GetSizeF();
	}

	void SplashScreen::Destruct()
	{
		m_resourceManager->RemoveListener(this);
	}

	void SplashScreen::Tick(float delta)
	{
		SetPos(Vector2::Zero);
		SetSize(GetWindowSize());

		const float target = static_cast<float>(m_loadedResourceCount) / static_cast<float>(m_totalResourceSize);
		m_progress		   = Math::Lerp(m_progress, target, delta * BAR_INTERP_SPEED);

		const float aspect	= m_logoTextureSize.x / m_logoTextureSize.y;
		m_logoDrawSize		= Vector2(GetSizeX() * 0.25f, GetSizeX() * 0.25f / aspect);
		const float padding = m_logoDrawSize.x * 0.1f;
		m_loadingBarHeight	= padding * 0.5f;

		m_logoCenter = Vector2(m_logoDrawSize.x / 2.0f + padding, m_logoDrawSize.y / 2.0f + padding);

		m_infoText1->SetPosX(padding);
		m_infoText1->SetPosY(m_logoCenter.y + m_logoDrawSize.y * 0.5f + padding);

		m_infoText2->SetPosX(padding);
		m_infoText2->SetPosY(m_infoText1->GetPosY() + m_infoText1->GetSizeY() + padding * 0.5f);

		m_versionText->SetPosX(padding);
		m_versionText->SetPosY(m_infoText2->GetPosY() + m_infoText2->GetSizeY() + padding * 0.5f);

		m_loadingInfo->SetPosX(m_rect.GetEnd().x - padding - m_loadingInfo->GetSizeX());
		m_loadingInfo->SetPosY(m_rect.GetEnd().y - m_loadingBarHeight * 2.0f - m_loadingInfo->GetHalfSizeY());
	}

	void SplashScreen::Draw()
	{
		const float padding = m_loadingBarHeight;

		LinaVG::StyleOptions opts;
		opts.color = Theme::GetDef().background1.AsLVG4();
		m_lvg->DrawRect(m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);

		// Logo
		m_lvg->DrawImage(m_splashImage->GetBindlessIndex() + 1, m_logoCenter.AsLVG(), m_logoDrawSize.AsLVG());

		const Rect progressBGRect = Rect(Vector2(m_rect.pos.x, m_rect.GetEnd().y - padding), Vector2(m_rect.size.x, padding));
		const Rect progressRect	  = Rect(Vector2(m_rect.pos.x, m_rect.GetEnd().y - padding), Vector2(m_rect.size.x * m_progress, padding));

		LinaVG::StyleOptions bg;
		bg.color = Theme::GetDef().background0.AsLVG4();
		m_lvg->DrawRect(progressBGRect.pos.AsLVG(), progressBGRect.GetEnd().AsLVG(), bg);

		LinaVG::StyleOptions progress;
		progress.color.start = Theme::GetDef().accentPrimary1.AsLVG4();
		progress.color.end	 = Theme::GetDef().accentPrimary0.AsLVG4();
		m_lvg->DrawRect(progressRect.pos.AsLVG(), progressRect.GetEnd().AsLVG(), progress);

		m_versionText->Draw();
		m_infoText1->Draw();
		m_infoText2->Draw();
		m_loadingInfo->Draw();
	}

	void SplashScreen::OnResourceLoadStarted(int32 taskID, const Vector<ResourceIdentifier>& idents)
	{
		m_totalResourceSize = static_cast<uint32>(idents.size());
	}

	void SplashScreen::OnResourceLoaded(int32 taskID, const ResourceIdentifier& ident)
	{
		LOCK_GUARD(m_loadMtx);
		m_loadingInfo->GetProps().text = ident.relativePath;
		m_loadingInfo->CalculateTextSize();
		m_loadedResourceCount++;
	}
} // namespace Lina::Editor
