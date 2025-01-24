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

#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Common/Math/Math.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void FoldLayout::CalculateSize(float delta)
	{
		if (m_children.empty())
			return;

		if (m_children.size() == 1)
		{
			m_children[0]->CalculateSize(delta);
			SetSizeY(m_children[0]->GetSizeY());
			return;
		}

		if (m_unfolded)
		{
			float childrenTotalHeight = 0.0f;
			for (auto* c : m_children)
			{
				if (c->GetFlags().IsSet(WF_HIDE))
					continue;

				c->CalculateSize(delta);
				childrenTotalHeight += c->GetSizeY() + GetWidgetProps().childPadding * m_manager->GetScalingFactor();
			}
			childrenTotalHeight -= GetWidgetProps().childPadding * m_manager->GetScalingFactor();
			SetSizeY(childrenTotalHeight);
		}
		else
			SetSizeY(m_children[0]->GetSizeY());
	}

	void FoldLayout::Tick(float delta)
	{
		Vector2		  start = GetStartFromMargins();
		const Vector2 end	= GetEndFromMargins();

		float x = start.x;
		float y = start.y;

		float tweenValue = 0.0f;

		if (m_props.useTween)
		{
			m_tween.Tick(delta);
			tweenValue = m_tween.GetValue();
		}

		size_t idx = 0;
		for (auto* c : m_children)
		{
			if (!m_unfolded && idx != 0)
			{
				c->GetFlags().Set(WF_HIDE);
				continue;
			}

			// c->GetFlags().Remove(WF_HIDE);
			c->GetFlags().Remove(WF_POS_ALIGN_X);
			c->SetPosX(WidgetManager::CalculateAlignedPosX(c));

			if (!c->GetFlags().IsSet(WF_HIDE))
			{
				c->SetPosY(y);
				y += c->GetSizeY() + GetWidgetProps().childPadding * m_manager->GetScalingFactor();
			}

			if (idx != 0)
			{
				c->SetPosX(c->GetPosX() + tweenValue);
			}

			idx++;
		}
	}

	bool FoldLayout::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (m_children.empty())
			return false;

		if (!m_children.front()->GetIsHovered())
			return false;

		if (m_props.doubleClickChangesFold && button == LINAGX_MOUSE_0 && act == LinaGX::InputAction::Repeated)
		{
			SetIsUnfolded(!GetIsUnfolded());
			return true;
		}

		if (m_props.lookForChevron && button == LINAGX_MOUSE_0 && act == LinaGX::InputAction::Pressed)
		{
			Widget* w		= m_children.front();
			Icon*	chevron = w->GetWidgetOfType<Icon>(w);

			if (chevron != nullptr && chevron->GetIsHovered())
			{
				SetIsUnfolded(!GetIsUnfolded());

				return true;
			}
		}

		return false;
	}

	void FoldLayout::SetIsUnfolded(bool unfolded)
	{
		if (unfolded == m_unfolded)
			return;

		m_unfolded = unfolded;

		if (m_props.onFoldChanged)
			m_props.onFoldChanged(m_unfolded);

		if (m_unfolded && m_props.useTween)
			m_tween = Tween(m_props.tweenPower, 0.0f, m_props.tweenDuration, TweenType::Bounce);

		uint32 i = 0;
		for (Widget* c : m_children)
		{
			if (i == 0)
			{
				i++;
				continue;
			}

			c->GetFlags().Set(WF_HIDE, !unfolded);
			i++;
		}
	}

} // namespace Lina
