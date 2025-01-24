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

#include "Editor/Widgets/FX/ProgressCircleFill.hpp"
#include "Editor/Editor.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Editor/Widgets/FX/LinaLoading.hpp"

namespace Lina::Editor
{
	void ProgressCircleFill::Construct()
	{
		GetWidgetProps().colorBackground = Theme::GetDef().foreground0;
	}
	void ProgressCircleFill::Initialize()
	{
		Widget::Initialize();

		if (m_linaLoading)
			return;

		if (m_props.includeLinaLoading)
		{
			LinaLoading* loading = m_manager->Allocate<LinaLoading>("Loading");
			loading->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			loading->SetAlignedPos(Vector2(0.5f, 0.5f));
			loading->SetAlignedSize(Vector2(0.5f, 0.5f));
			loading->SetAnchorX(Anchor::Center);
			loading->SetAnchorY(Anchor::Center);
			m_linaLoading = loading;
			AddChild(loading);
		}
	}

	void ProgressCircleFill::Tick(float delta)
	{
		if (m_props.rotate)
			m_rotation += delta * m_props.barSpeed;
		else
			m_progress = Math::EaseOut(m_progress, m_targetProgress, delta * m_props.barSpeed);
	}

	void ProgressCircleFill::Draw()
	{
		LinaVG::StyleOptions opts;
		opts.isFilled	= false;
		opts.aaEnabled	= true;
		opts.color		= m_props.rotate ? m_props.colorProgress.AsLVG4() : GetWidgetProps().colorBackground.AsLVG();
		opts.thickness	= m_props.thickness;
		opts.rounding	= 0.0f;
		const float rad = Math::Min(m_rect.size.x, m_rect.size.y) * 0.5f;

		m_lvg->DrawCircle(m_rect.GetCenter().AsLVG(), rad, opts, 36, m_rotation, 100.0f, 450.0f, m_drawOrder);

		if (!m_props.rotate)
		{
			opts.color = m_props.colorProgress.AsLVG4();
			m_lvg->DrawCircle(m_rect.GetCenter().AsLVG(), rad, opts, 36, 0.0f, 100.0f, 101.0f + 349.0f * m_progress, m_drawOrder + 1);
		}
	}

	void ProgressCircleFill::UpdateProgress(float prog)
	{
		m_targetProgress = prog;
	}

} // namespace Lina::Editor
