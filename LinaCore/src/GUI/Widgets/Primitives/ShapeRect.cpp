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

#include "Core/GUI/Widgets/Primitives/ShapeRect.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Utility/TextureAtlas.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{

	void ShapeRect::Initialize()
	{
		Widget::Initialize();
		m_usedColorStart = m_props.colorStart;
		m_usedColorEnd	 = m_props.colorEnd;
	}

	void ShapeRect::Tick(float dt)
	{
		if (m_props.interpolateColor)
		{
			m_usedColorStart = Math::Lerp(m_usedColorStart, m_props.colorStart, dt * m_props.colorInterpolateSpeed);
			m_usedColorEnd	 = Math::Lerp(m_usedColorEnd, m_props.colorEnd, dt * m_props.colorInterpolateSpeed);
		}
		else
		{
			m_usedColorStart = m_props.colorStart;
			m_usedColorEnd	 = m_props.colorEnd;
		}
	}

	void ShapeRect::Draw()
	{
		if (!GetIsVisible())
			return;

		LinaVG::StyleOptions opts;
		opts.color.start				= m_usedColorStart.AsLVG4();
		opts.color.end					= m_usedColorEnd.AsLVG4();
		opts.color.gradientType			= m_props.colorGradientHorizontal ? LinaVG::GradientType::Horizontal : LinaVG::GradientType::Vertical;
		opts.outlineOptions.color.start = opts.outlineOptions.color.end = m_props.colorOutline.AsLVG4();
		opts.rounding													= m_props.rounding;
		opts.outlineOptions.thickness									= m_props.outlineThickness;

		for (int corner : m_props.onlyRoundCorners)
			opts.onlyRoundTheseCorners.push_back(corner);

		Vector2		  min = m_rect.pos;
		Vector2		  max = m_rect.GetEnd();
		const Vector2 sz  = max - min;

		if (m_props.imageTexture != nullptr || m_props.imageTextureAtlas != nullptr)
		{
			const Vector2 start = GetStartFromMargins();
			const Vector2 end	= GetEndFromMargins();
			Vector2		  size	= end - start;

			if (m_props.fitImage)
			{
				const Vector2 txtSize = m_props.imageTextureAtlas ? Vector2(static_cast<float>(m_props.imageTextureAtlas->rectUV.size.x), static_cast<float>(m_props.imageTextureAtlas->rectUV.size.y)) : m_props.imageTexture->GetSizeF();
				const float	  max	  = Math::Max(txtSize.x, txtSize.y);
				const float	  min	  = Math::Min(txtSize.x, txtSize.y);
				const float	  aspect  = max / min;

				if (txtSize.x > txtSize.y)
					size.y = size.x / aspect;
				else
					size.x = size.y / aspect;
			}

			if (m_props.imageTexture != nullptr)
			{
				if (m_props.imageTexture->GetMeta().format == LinaGX::Format::R8_UNORM)
					opts.color.start.w = opts.color.end.w = GUI_IS_SINGLE_CHANNEL;

				opts.textureHandle = m_props.imageTexture->GetBindlessIndex() + 1;
			}
			else if (m_props.imageTextureAtlas != nullptr)
			{
				if (m_props.imageTextureAtlas->atlas->GetRaw()->GetMeta().format == LinaGX::Format::R8_UNORM)
					opts.color.start.w = opts.color.end.w = GUI_IS_SINGLE_CHANNEL;

				opts.textureHandle	 = m_props.imageTextureAtlas->atlas->GetRaw()->GetBindlessIndex() + 1;
				opts.textureUVOffset = m_props.imageTextureAtlas->rectUV.pos.AsLVG();
				opts.textureUVTiling = m_props.imageTextureAtlas->rectUV.size.AsLVG();
			}

			if (size.x < sz.x)
			{
				const float diff = sz.x - size.x;
				min.x += diff * 0.5f;
				max.x -= diff * 0.5f;
			}

			if (size.y < sz.y)
			{
				const float diff = sz.y - size.y;
				min.y += diff * 0.5f;
				max.y -= diff * 0.5f;
			}
		}

		m_lvg->DrawRect(min.AsLVG(), max.AsLVG(), opts, 0.0f, m_drawOrder);

		Widget::Draw();
		Widget::DrawTooltip();
	}

	bool ShapeRect::OnMouse(uint32 button, LinaGX::InputAction action)
	{
		if (m_props.onRightClicked && button == LINAGX_MOUSE_1 && action == LinaGX::InputAction::Pressed && m_isHovered)
		{
			m_props.onRightClicked();
			return true;
		}

		if (button != LINAGX_MOUSE_0)
			return false;

		if (m_props.onDoubleClicked && m_isHovered && action == LinaGX::InputAction::Repeated)
		{
			m_props.onDoubleClicked();
			return true;
		}

		if (m_props.onClicked && m_isHovered && action == LinaGX::InputAction::Pressed)
		{
			m_props.onClicked();
			return true;
		}

		return false;
	}
} // namespace Lina
