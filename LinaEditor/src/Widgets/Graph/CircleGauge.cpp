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

#include "Editor/Widgets/Graph/CircleGauge.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Primitives/Circle.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"

namespace Lina::Editor
{
	void CircleGauge::Construct()
	{
		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSize(Vector2::One);
		layout->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		layout->GetProps().direction		  = DirectionOrientation::Vertical;

		m_circle = m_manager->Allocate<Circle>("Circle");
		m_circle->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_circle->SetAlignedPosX(0.0f);
		m_circle->SetAlignedSize(Vector2(1.0f, 0.0));
		layout->AddChild(m_circle);

		m_dataText = m_manager->Allocate<Text>("Title");
		m_dataText->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		m_dataText->SetAlignedPos(Vector2(0.5f, 0.75f));
		m_dataText->SetAnchorX(Anchor::Center);
		m_dataText->SetAnchorY(Anchor::Center);
		m_circle->AddChild(m_dataText);

		m_title = m_manager->Allocate<Text>("Title");
		m_title->GetFlags().Set(WF_POS_ALIGN_X);
		m_title->SetAlignedPosX(0.5f);
		m_title->SetAnchorX(Anchor::Center);
		layout->AddChild(m_title);

		AddChild(layout);
	}

} // namespace Lina::Editor
