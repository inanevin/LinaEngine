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
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{

	void ShapeRect::Draw()
	{
		if (!GetIsVisible())
			return;

		LinaVG::StyleOptions opts;
		opts.color.start				= m_props.colorStart.AsLVG4();
		opts.color.end					= m_props.colorEnd.AsLVG4();
		opts.outlineOptions.color.start = opts.outlineOptions.color.end = m_props.colorOutline.AsLVG4();
		opts.rounding													= m_props.rounding;
		opts.outlineOptions.thickness									= m_props.outlineThickness;

		if (m_props.imageTexture != nullptr)
		{
			const Vector2 start = GetStartFromMargins();
			const Vector2 end	= GetEndFromMargins();
			Vector2		  size	= end - start;

			if (m_props.fitImage)
			{
				const Vector2 txtSize = m_props.imageTexture->GetSizeF();
				const float	  max	  = Math::Max(txtSize.x, txtSize.y);
				const float	  min	  = Math::Min(txtSize.x, txtSize.y);
				const float	  aspect  = max / min;

				if (txtSize.x > txtSize.y)
					size.y = size.x / aspect;
				else
					size.x = size.y / aspect;
			}

			Color color = m_props.colorStart;

			if (m_props.imageTexture->GetMeta().format == LinaGX::Format::R8_UNORM)
				color.w = GUI_IS_SINGLE_CHANNEL;
			m_lvg->DrawImage(m_props.imageTexture->GetBindlessIndex() + 1, m_rect.GetCenter().AsLVG(), size.AsLVG(), color.AsLVG4(), 0.0f, m_drawOrder);
		}
		else
			m_lvg->DrawRect(m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);

		Widget::Draw();
		Widget::DrawTooltip();
	}
} // namespace Lina
