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

#include "Editor/Widgets/FX/LinaLoading.hpp"
#include "Editor/Editor.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Core/Graphics/Utility/TextureAtlas.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Application.hpp"

namespace Lina::Editor
{
	void LinaLoading::Construct()
	{
		m_imgLeft	= Editor::Get()->GetApp()->GetResourceManager().GetResource<Texture>(EDITOR_TEXTURE_LINA_LOGO_LEFT_ID);
		m_imgRight	= Editor::Get()->GetApp()->GetResourceManager().GetResource<Texture>(EDITOR_TEXTURE_LINA_LOGO_RIGHT_ID);
		m_imgBottom = Editor::Get()->GetApp()->GetResourceManager().GetResource<Texture>(EDITOR_TEXTURE_LINA_LOGO_BOTTOM_ID);
	}

	void LinaLoading::Tick(float delta)
	{
		const float disperseAmt = (m_rect.GetEnd() - m_rect.pos).MagnitudeSqrt() * m_props.dispersePower * 0.005f;
		if (m_action == -1)
		{
			m_tween = Tween(0.0f, 1.0f, m_props.tweenTime, TweenType::Sinusoidal);
			m_tween.SetDelay(m_props.waitTime);
			m_action = 0;
		}

		m_tween.Tick(delta);

		if (m_action == 0)
		{
			m_rightOffset = Math::Lerp(Vector2::Zero, Vector2(1.0f, -0.6f) * disperseAmt, m_tween.GetValue());
			m_rightTint	  = Math::Lerp(Theme::GetDef().foreground1, Theme::GetDef().accentPrimary2, m_tween.GetValue());
		}
		else if (m_action == 1)
		{
			m_bottomOffset = Math::Lerp(Vector2::Zero, Vector2(0.0f, 1.0f) * disperseAmt, m_tween.GetValue());
			m_bottomTint   = Math::Lerp(Theme::GetDef().foreground1, Theme::GetDef().accentPrimary2, m_tween.GetValue());
		}
		else if (m_action == 2)
		{
			m_leftOffset = Math::Lerp(Vector2::Zero, Vector2(-1.0f, -1.0f) * disperseAmt, m_tween.GetValue());
			m_leftTint	 = Math::Lerp(Theme::GetDef().foreground1, Theme::GetDef().accentPrimary2, m_tween.GetValue());
		}
		else if (m_action == 3)
		{
			m_rightOffset  = Math::Lerp(Vector2::Zero, Vector2(1.0f, -1.0f) * disperseAmt, m_tween.GetValue());
			m_bottomOffset = Math::Lerp(Vector2::Zero, Vector2(0.0f, 1.0f) * disperseAmt, m_tween.GetValue());
			m_leftOffset   = Math::Lerp(Vector2::Zero, Vector2(-1.0f, -1.0f) * disperseAmt, m_tween.GetValue());

			m_leftTint	 = Math::Lerp(Theme::GetDef().foreground1, Theme::GetDef().accentPrimary2, m_tween.GetValue());
			m_rightTint	 = Math::Lerp(Theme::GetDef().foreground1, Theme::GetDef().accentPrimary2, m_tween.GetValue());
			m_bottomTint = Math::Lerp(Theme::GetDef().foreground1, Theme::GetDef().accentPrimary2, m_tween.GetValue());
		}

		if (m_tween.GetIsCompleted())
		{
			m_action = (m_action + 1) % 4;

			if (m_action == 3)
			{
				m_tween.SetStart(1.0f);
				m_tween.SetEnd(0.0f);
				m_tween.SetTween(TweenType::Exponential);
			}
			else
			{
				m_tween.SetStart(0.0f);
				m_tween.SetEnd(1.0f);
				m_tween.SetTween(TweenType::Sinusoidal);
			}

			m_tween.Restart();
		}
	}

	void LinaLoading::Draw()
	{
		LinaVG::StyleOptions opts;

		auto draw = [&](Texture* img, const Vector2& offset, const Color& color) {
			opts.textureHandle = img;
			// opts.textureUVOffset = img->rectUV.pos.AsLVG();
			// opts.textureUVTiling = img->rectUV.size.AsLVG();
			opts.color = color.AsLVG4();

			const Vector2 start = m_rect.pos + offset;
			const Vector2 end	= start + (m_rect.GetEnd() - m_rect.pos);
			m_lvg->DrawRect(start.AsLVG(), end.AsLVG(), opts, 0.0f, m_drawOrder);
		};

		draw(m_imgLeft, m_leftOffset, m_leftTint);
		draw(m_imgRight, m_rightOffset, m_rightTint);
		draw(m_imgBottom, m_bottomOffset, m_bottomTint);
	}
} // namespace Lina::Editor
