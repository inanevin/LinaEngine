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

#include "Core/GUI/Widgets/Compound/Popup.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{

	void Popup::Destruct()
	{
		if (m_props.onDestructed)
			m_props.onDestructed();
	}

	void Popup::Tick(float delta)
	{
		float totalHeight = GetChildMargins().top;

		for (auto* c : m_children)
		{
			c->SetPosY(m_rect.pos.y + totalHeight);
			totalHeight += c->GetSize().y;
		}

		SetSizeY(totalHeight + GetChildMargins().bottom);

		if (m_animationCtr < m_props.animTime)
		{
			m_animationCtr += delta;
			const float animAlpha = Math::Remap(m_animationCtr, 0.0f, m_props.animTime, 0.0f, 1.0f);
			m_rect.size.y		  = Math::Lerp(0.0f, totalHeight + GetChildMargins().bottom, animAlpha);
		}
	}

	void Popup::Draw(int32 threadIndex)
	{
		LinaVG::StyleOptions opts;
		opts.color.start			  = m_props.colorBackgroundStart.AsLVG4();
		opts.color.end				  = m_props.colorBackgroundEnd.AsLVG4();
		opts.color.gradientType		  = LinaVG::GradientType::Vertical;
		opts.outlineOptions.color	  = m_props.colorOutline.AsLVG4();
        
        Color ds = Theme::GetDef().black;
        ds.w = 0.5f;
        WidgetUtility::DrawDropShadowRect(threadIndex, m_rect, m_drawOrder, ds, 6);
        LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);

		m_manager->SetClip(threadIndex, m_rect, {});
		uint32 idx = 0;
		for (auto* c : m_children)
		{
			c->SetChildID(idx);
			c->Draw(threadIndex);
		}
		m_manager->UnsetClip(threadIndex);
	}

} // namespace Lina
