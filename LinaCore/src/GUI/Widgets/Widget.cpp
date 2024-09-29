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

#include "Core/GUI/Widgets/Widget.hpp"
#include "Core/Graphics/Utility/TextureAtlas.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void Widget::ChangedParent(Widget* w)
	{
		w->m_lgxWindow = w->m_parent->m_lgxWindow;
		w->m_manager   = w->m_parent->m_manager;
		w->m_lvg	   = w->m_parent->m_lvg;

		for (auto* c : w->GetChildren())
			ChangedParent(c);
	}

	void Widget::SetWidgetManager(WidgetManager* wm)
	{
		m_manager	= wm;
		m_lvg		= wm->GetLVG();
		m_lgxWindow = wm->GetRoot()->GetWindow();
	}

	void Widget::AddChild(Widget* w)
	{
		w->m_parent	   = this;
		w->m_lgxWindow = m_lgxWindow;

		if (m_manager != nullptr)
		{
			w->m_lvg			 = m_manager->GetLVG();
			w->m_resourceManager = m_manager->GetResourceManagerV2();
		}

		for (auto* c : w->GetChildren())
			ChangedParent(c);

		const size_t cur = m_children.size();

		Widget* last = m_children.empty() ? nullptr : m_children[m_children.size() - 1];
		if (last != nullptr)
		{
			last->m_next = w;
			w->m_prev	 = last;
		}

		w->m_next = nullptr;
		m_children.push_back(w);
	}

	void Widget::ExecuteNextFrame(Delegate<void()>&& cb)
	{
		m_executeNextFrame.push_back(cb);
	}

	void Widget::RemoveChild(Widget* w)
	{
		if (w->m_prev != nullptr)
		{
			w->m_prev->m_next = w->m_next;
		}
		if (w->m_next != nullptr)
		{
			w->m_next->m_prev = w->m_prev;
		}

		// w->m_controlsManager = nullptr;
		// w->m_controlsOwner	 = nullptr;

		auto it = linatl::find_if(m_children.begin(), m_children.end(), [w](Widget* child) -> bool { return w == child; });

		LINA_ASSERT(it != m_children.end(), "");
		m_children.erase(it);

		w->m_next = w->m_prev = nullptr;
	}

	void Widget::DeallocAllChildren()
	{
		for (auto* c : m_children)
			m_manager->Deallocate(c);
	}

	void Widget::RemoveAllChildren()
	{
		m_children.clear();
	}

	void Widget::Initialize()
	{
		m_initializing					 = true;
		m_widgetProps._interpolatedColor = m_widgetProps.colorBackground;
		m_manager->InitializeWidget(this);
		linatl::for_each(m_children.begin(), m_children.end(), [this](Widget* child) -> void {
			child->Initialize();
			if (GetIsDisabled())
				child->SetIsDisabled(true);
		});
		m_initializing = false;
	}

	void Widget::DrawDropshadow()
	{
		if (!m_widgetProps.dropshadow.enabled)
			return;

		const float			 startAlpha = m_widgetProps.dropshadow.color.w;
		LinaVG::StyleOptions opts;
		opts.isFilled = true;
		opts.rounding = m_widgetProps.dropshadow.rounding;
		opts.color	  = m_widgetProps.dropshadow.color.AsLVG4();

		for (int corner : m_widgetProps.dropshadow.onlyRound)
			opts.onlyRoundTheseCorners.push_back(corner);

		if (m_widgetProps.dropshadow.direction == Direction::Center)
		{
			Vector2 min = m_rect.pos;
			Vector2 max = m_rect.GetEnd();

			for (int32 i = 0; i < m_widgetProps.dropshadow.steps; i++)
			{
				m_lvg->DrawRect(min.AsLVG(), max.AsLVG(), opts, 0.0f, m_drawOrder);
				min -= Vector2(m_widgetProps.dropshadow.thickness);
				max += Vector2(m_widgetProps.dropshadow.thickness);
				opts.color.start.w = opts.color.end.w = Math::Lerp(startAlpha, 0.0f, static_cast<float>(i) / static_cast<float>(m_widgetProps.dropshadow.steps));
			}
		}
		else
		{
			const Vector2 dir = DirectionToVector(m_widgetProps.dropshadow.direction);
			Vector2		  min = Vector2::Zero, max = Vector2::Zero;

			if (m_widgetProps.dropshadow.direction == Direction::Bottom)
			{
				min = Vector2(m_rect.pos.x + m_widgetProps.dropshadow.margin, m_rect.GetEnd().y);
				max = Vector2(m_rect.GetEnd().x - m_widgetProps.dropshadow.margin, m_rect.GetEnd().y + m_widgetProps.dropshadow.thickness);
			}
			else if (m_widgetProps.dropshadow.direction == Direction::Top)
			{
				min = m_rect.pos + Vector2(m_widgetProps.dropshadow.margin, 0.0f);
				max = Vector2(m_rect.GetEnd().x - m_widgetProps.dropshadow.margin, m_rect.pos.y - m_widgetProps.dropshadow.thickness);
			}
			else if (m_widgetProps.dropshadow.direction == Direction::Right)
			{
				min = Vector2(m_rect.GetEnd().x, m_rect.pos.y + m_widgetProps.dropshadow.margin);
				max = Vector2(m_rect.GetEnd().x + m_widgetProps.dropshadow.thickness, m_rect.GetEnd().y - m_widgetProps.dropshadow.margin);
			}
			else
			{
				min = Vector2(m_rect.pos.x - m_widgetProps.dropshadow.thickness, m_rect.pos.y + m_widgetProps.dropshadow.margin);
				max = Vector2(m_rect.pos.x, m_rect.GetEnd().y - m_widgetProps.dropshadow.margin);
			}

			if (m_widgetProps.dropshadow.isInner)
			{
				min -= dir * m_widgetProps.dropshadow.thickness;
				max -= dir * m_widgetProps.dropshadow.thickness;
			}

			for (int32 i = 0; i < m_widgetProps.dropshadow.steps; i++)
			{
				m_lvg->DrawRect(min.AsLVG(), max.AsLVG(), opts, 0.0f, m_drawOrder + m_widgetProps.dropshadow.drawOrderIncrement);
				min += m_widgetProps.dropshadow.thickness * dir * (m_widgetProps.dropshadow.isInner ? -1.0f : 1.0f);
				max += m_widgetProps.dropshadow.thickness * dir * (m_widgetProps.dropshadow.isInner ? -1.0f : 1.0f);
				opts.color.start.w = opts.color.end.w = Math::Lerp(startAlpha, 0.0f, static_cast<float>(i) / static_cast<float>(m_widgetProps.dropshadow.steps));
			}
		}
	}
	void Widget::DrawBackground()
	{
		if (m_widgetProps.drawBackground)
		{
			Vector2				 size = m_rect.GetEnd() - m_rect.pos;
			LinaVG::StyleOptions opts;
			opts.color						  = m_widgetProps.colorBackground.AsLVG();
			opts.outlineOptions.thickness	  = m_widgetProps.outlineThickness;
			opts.outlineOptions.drawDirection = m_widgetProps.outlineIsInner ? LinaVG::OutlineDrawDirection::Inwards : LinaVG::OutlineDrawDirection::Outwards;
			opts.outlineOptions.color		  = m_manager->IsControlsOwner(this) ? m_widgetProps.colorOutlineControls.AsLVG() : m_widgetProps.colorOutline.AsLVG();
			opts.rounding = m_widgetProps.rounding = m_widgetProps.rounding;

			if (m_widgetProps.hoveredIsDifferentColor && m_isHovered)
				opts.color = m_widgetProps.colorHovered.AsLVG();

			if (m_widgetProps.pressedIsDifferentColor && m_isPressed)
				opts.color = m_widgetProps.colorPressed.AsLVG();

			if (GetIsDisabled())
				opts.color = m_widgetProps.colorDisabled.AsLVG();

			if (!m_widgetProps.onlyRound.empty())
			{
				for (int32 corner : m_widgetProps.onlyRound)
					opts.onlyRoundTheseCorners.push_back(corner);
			}

			Vector2 min = m_rect.pos;
			Vector2 max = m_rect.GetEnd();
			Vector2 sz	= m_rect.GetEnd() - m_rect.pos;

			if (m_widgetProps.useSpecialTexture)
			{
				opts.textureHandle = m_widgetProps.specialTexture;
			}
			else if (m_widgetProps.textureAtlas != nullptr || m_widgetProps.rawTexture != nullptr)
			{
				Vector2 size		  = m_rect.GetEnd() - m_rect.pos;
				Vector2 textureTiling = m_widgetProps.textureTiling;
				Vector2 textureSize	  = Vector2::Zero;
				if (m_widgetProps.fitTexture)
				{
					const Vector2 txtSize = m_widgetProps.textureAtlas ? Vector2(static_cast<float>(m_widgetProps.textureAtlas->rectUV.size.x), static_cast<float>(m_widgetProps.textureAtlas->rectUV.size.y)) : m_widgetProps.rawTexture->GetSizeF();
					// const float	  max	  = Math::Max(txtSize.x, txtSize.y);
					// const float	  min	  = Math::Min(txtSize.x, txtSize.y);
					// const float	  aspect  = txtSize.x / txtSize.y;

					const float widgetAspect  = sz.x / sz.y;
					const float textureAspect = txtSize.x / txtSize.y;

					Vector2 scaledSize = Vector2::Zero;

					if (widgetAspect > textureAspect)
						scaledSize = Vector2(sz.y * textureAspect, sz.y);
					else
						scaledSize = Vector2(sz.x, sz.x / textureAspect);

					const Vector2 off = (sz - scaledSize) * 0.5f;
					min				  = min + off;
					max				  = min + scaledSize;

					// if (txtSize.x > txtSize.y)
					//	size.y = size.x / aspect;
					// else
					//	size.x = size.y / aspect;
				}

				if (m_widgetProps.rawTexture != nullptr)
				{
					if (m_widgetProps.rawTexture->GetMeta().format == LinaGX::Format::R8_UNORM)
						opts.color.start.w = opts.color.end.w = GUI_IS_SINGLE_CHANNEL;
					opts.textureHandle = m_widgetProps.rawTexture;
					opts.userData	   = m_widgetProps.textureUserData;
					textureSize		   = m_widgetProps.rawTexture->GetSizeF();
				}
				else if (m_widgetProps.textureAtlas != nullptr)
				{
					if (m_widgetProps.textureAtlas->atlas->GetRaw()->GetMeta().format == LinaGX::Format::R8_UNORM)
						opts.color.start.w = opts.color.end.w = GUI_IS_SINGLE_CHANNEL;
					opts.userData		 = m_widgetProps.textureUserData;
					opts.textureHandle	 = m_widgetProps.textureAtlas->atlas->GetRaw();
					opts.textureUVOffset = m_widgetProps.textureAtlas->rectUV.pos.AsLVG();
					textureTiling *= m_widgetProps.textureAtlas->rectUV.size;
					textureSize = m_widgetProps.textureAtlas->rectUV.size * m_widgetProps.textureAtlas->atlas->GetRaw()->GetSizeF();
				}

				if (m_widgetProps.activeTextureTiling)
				{
					const Vector2 ratio	 = m_rect.size / textureSize;
					const float	  aspect = m_rect.size.x / m_rect.size.y;
					textureTiling.y /= aspect;
				}

				opts.textureUVTiling = textureTiling.AsLVG();

				// if (size.x < sz.x)
				// {
				//     const float diff = sz.x - size.x;
				//     min.x += diff * 0.5f;
				//     max.x -= diff * 0.5f;
				// }
				//
				// if (size.y < sz.y)
				// {
				//     const float diff = sz.y - size.y;
				//     min.y += diff * 0.5f;
				//     max.y -= diff * 0.5f;
				// }
			}

			if (m_widgetProps.interpolateColor)
			{
				m_widgetProps._interpolatedColor.start = Math::Lerp(m_widgetProps._interpolatedColor.start, Color(opts.color.start), SystemInfo::GetDeltaTimeF() * m_widgetProps.colorInterpolateSpeed);
				m_widgetProps._interpolatedColor.end   = Math::Lerp(m_widgetProps._interpolatedColor.end, Color(opts.color.end), SystemInfo::GetDeltaTimeF() * m_widgetProps.colorInterpolateSpeed);
				opts.color							   = m_widgetProps._interpolatedColor.AsLVG();
			}
			opts.color.gradientType = m_widgetProps.colorBackgroundDirection == DirectionOrientation::Horizontal ? LinaVG::GradientType::Horizontal : LinaVG::GradientType::Vertical;

			if (m_widgetProps.backgroundIsCentralGradient)
			{
				LinaVG::StyleOptions style	= opts;
				LinaVG::StyleOptions style2 = opts;
				style.color.start			= m_widgetProps.colorBackground.start.AsLVG4();
				style.color.end				= m_widgetProps.colorBackground.end.AsLVG4();
				style2.color.start			= m_widgetProps.colorBackground.end.AsLVG4();
				style2.color.end			= m_widgetProps.colorBackground.start.AsLVG4();
				m_lvg->DrawRect(GetPos().AsLVG(), Vector2((m_rect.GetEnd().x + GetPos().x) * 0.5f, m_rect.GetEnd().y).AsLVG(), style, 0.0f, m_drawOrder);
				m_lvg->DrawRect(Vector2((m_rect.GetEnd().x + GetPos().x) * 0.5f, GetPos().y).AsLVG(), m_rect.GetEnd().AsLVG(), style2, 0.0f, m_drawOrder);
			}
			else
				m_lvg->DrawRect(min.AsLVG(), max.AsLVG(), opts, 0.0f, m_drawOrder);
		}
	}

	void Widget::DrawChildren()
	{
		if (m_widgetProps.clipChildren)
			m_manager->SetClip(m_widgetProps.customClip ? m_widgetProps.customClipRect : m_rect, {});

		linatl::for_each(m_children.begin(), m_children.end(), [](Widget* child) -> void {
			if (child->GetIsVisible() && !child->GetFlags().IsSet(WF_HIDE))
				child->Draw();
		});

		if (m_widgetProps.clipChildren)
			m_manager->UnsetClip();
	}

	void Widget::Draw()
	{
		DrawBackground();
		DrawDropshadow();
		DrawChildren();
		DrawBorders();
		DrawTooltip();
	}

	void DropshadowProps::SaveToStream(OStream& stream) const
	{
		stream << static_cast<uint32>(DS_VERSION);
		stream << enabled << isInner;
		stream << margin << thickness << rounding << drawOrderIncrement << steps;
		stream << color;
		stream << direction;
		stream << onlyRound;
	}

	void DropshadowProps::LoadFromStream(IStream& stream)
	{
		uint32 version = 0;
		stream >> version;
		stream >> enabled >> isInner;
		stream >> margin >> thickness >> rounding >> drawOrderIncrement >> steps;
		stream >> color;
		stream >> direction;
		stream >> onlyRound;
	}

	void WidgetProps::SaveToStream(OStream& stream) const
	{
		stream << static_cast<uint32>(WIDGETPROPS_VERSION);
		stream << dropshadow;
		stream << clipChildren << customClip << drawBackground << backgroundIsCentralGradient;
		stream << interpolateColor << fitTexture << hoveredIsDifferentColor << pressedIsDifferentColor;
		stream << activeTextureTiling << useSpecialTexture << outlineIsInner;
		stream << colorInterpolateSpeed << outlineThickness << rounding << childPadding;
		stream << drawOrderIncrement;
		stream << tooltip;
		stream << debugName;
		stream << childMargins.top << childMargins.bottom << childMargins.left << childMargins.right;
		stream << borderThickness.top << borderThickness.bottom << borderThickness.left << borderThickness.right;
		stream << colorBorders;
		stream << colorBackground;
		stream << colorOutline;
		stream << colorOutlineControls;
		stream << colorHovered;
		stream << colorPressed;
		stream << colorDisabled;
		stream << colorBackgroundDirection;
		stream << specialTexture;
		stream << textureTiling;
		stream << onlyRound;
		stream << customClipRect;
	}

	void WidgetProps::LoadFromStream(IStream& stream)
	{
		uint32 version = 0;
		stream >> version;
		stream >> dropshadow;
		stream >> clipChildren >> customClip >> drawBackground >> backgroundIsCentralGradient;
		stream >> interpolateColor >> fitTexture >> hoveredIsDifferentColor >> pressedIsDifferentColor;
		stream >> activeTextureTiling >> useSpecialTexture >> outlineIsInner;
		stream >> colorInterpolateSpeed >> outlineThickness >> rounding >> childPadding;
		stream >> drawOrderIncrement;
		stream >> tooltip;
		stream >> debugName;
		stream >> childMargins.top >> childMargins.bottom >> childMargins.left >> childMargins.right;
		stream >> borderThickness.top >> borderThickness.bottom >> borderThickness.left >> borderThickness.right;
		stream >> colorBorders;
		stream >> colorBackground;
		stream >> colorOutline;
		stream >> colorOutlineControls;
		stream >> colorHovered;
		stream >> colorPressed;
		stream >> colorDisabled;
		stream >> colorBackgroundDirection;
		stream >> specialTexture;
		stream >> textureTiling;
		stream >> onlyRound;
		stream >> customClipRect;
	}

	void Widget::SaveToStream(OStream& stream) const
	{
		stream << static_cast<uint32>(WIDGET_VERSION);
		stream << m_tid;
		stream << m_widgetProps;
		stream << m_flags;
		stream << m_rect;
		stream << m_alignedPos;
		stream << m_alignedSize;
		stream << m_fixedSize;
		stream << m_anchorX << m_anchorY;

		const uint32 childSz = static_cast<uint32>(m_children.size());
		stream << childSz;
		for (uint32 i = 0; i < childSz; i++)
		{
			auto* c = m_children[i];
			stream << c->GetTID();
			c->SaveToStream(stream);
		}
	}

	void Widget::LoadFromStream(IStream& stream)
	{
		DeallocAllChildren();
		RemoveAllChildren();

		stream >> m_loadedVersion;
		stream >> m_tid;
		stream >> m_widgetProps;
		stream >> m_flags;
		stream >> m_rect;
		stream >> m_alignedPos;
		stream >> m_alignedSize;
		stream >> m_fixedSize;
		stream >> m_anchorX >> m_anchorY;

		uint32 childSz = 0;
		stream >> childSz;

		for (uint32 i = 0; i < childSz; i++)
		{
			TypeID tid = 0;
			stream >> tid;
			Widget* c = m_manager->Allocate(tid);
			c->LoadFromStream(stream);
			AddChild(c);
		}
	}

	void Widget::DrawBorders()
	{
		LinaVG::StyleOptions border;
		border.color = m_widgetProps.colorBorders.AsLVG4();

		if (!Math::Equals(m_widgetProps.borderThickness.left, 0.0f, 0.5f))
		{
			const Vector2 start = m_rect.pos;
			const Vector2 end	= start + Vector2(0, m_rect.size.y);
			border.thickness	= m_widgetProps.borderThickness.left;
			m_lvg->DrawLine(start.AsLVG(), end.AsLVG(), border, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);
		}

		if (!Math::Equals(m_widgetProps.borderThickness.right, 0.0f, 0.5f))
		{
			const Vector2 start = m_rect.pos + Vector2(m_rect.size.x, 0.0f);
			const Vector2 end	= start + Vector2(0, m_rect.size.y);
			border.thickness	= m_widgetProps.borderThickness.right;
			m_lvg->DrawLine(start.AsLVG(), end.AsLVG(), border, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);
		}

		if (!Math::Equals(m_widgetProps.borderThickness.top, 0.0f, 0.5f))
		{
			const Vector2 start = m_rect.pos;
			const Vector2 end	= start + Vector2(m_rect.size.x, 0.0f);
			border.thickness	= m_widgetProps.borderThickness.top;
			m_lvg->DrawLine(start.AsLVG(), end.AsLVG(), border, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);
		}

		if (!Math::Equals(m_widgetProps.borderThickness.bottom, 0.0f, 0.5f))
		{
			const Vector2 start = m_rect.pos + Vector2(0.0f, m_rect.size.y);
			const Vector2 end	= start + Vector2(m_rect.size.x, 0.0f);
			border.thickness	= m_widgetProps.borderThickness.bottom;
			m_lvg->DrawLine(start.AsLVG(), end.AsLVG(), border, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);
		}
	}

	void Widget::DrawTooltip()
	{

		const Vector2 mp = Vector2(Math::FloorToFloat(m_lgxWindow->GetMousePosition().x), Math::FloorToFloat(m_lgxWindow->GetMousePosition().y));

		if (!m_isHovered)
			return;

		if (m_customTooltip)
		{
			m_customTooltip->SetPos(mp + Vector2(10, 10));
			return;
		}

		const String& tooltip = GetWidgetProps().tooltip;

		if (tooltip.empty())
			return;

		LinaVG::TextOptions textOpts;
		textOpts.font		   = m_manager->GetDefaultFont()->GetFont(m_lgxWindow->GetDPIScale());
		const Vector2 textSize = m_lvg->CalculateTextSize(tooltip.c_str(), textOpts);

		const Rect tooltipRect = Rect(mp + Vector2(10, 10), textSize + Vector2(Theme::GetDef().baseIndent * 2.0f, Theme::GetDef().baseIndent));

		LinaVG::StyleOptions bg;
		bg.color					= Theme::GetDef().background1.AsLVG4();
		bg.outlineOptions.thickness = Theme::GetDef().baseOutlineThickness;
		bg.outlineOptions.color		= Theme::GetDef().black.AsLVG4();
		m_lvg->DrawRect(tooltipRect.pos.AsLVG(), tooltipRect.GetEnd().AsLVG(), bg, 0.0f, TOOLTIP_DRAW_ORDER);

		m_lvg->DrawTextNormal(tooltip.c_str(), Vector2(tooltipRect.pos.x + Theme::GetDef().baseIndent, tooltipRect.GetCenter().y + textSize.y * 0.5f).AsLVG(), textOpts, 0.0f, TOOLTIP_DRAW_ORDER);
	}

	void Widget::DebugDraw(int32 drawOrder)
	{
		linatl::for_each(m_children.begin(), m_children.end(), [drawOrder](Widget* child) -> void { child->DebugDraw(drawOrder); });
	}

	void Widget::SetIsHovered()
	{
		if (m_isDisabled)
		{
			m_isHovered = false;
			return;
		}

		auto*		foregroundRoot	= m_manager->GetForegroundRoot();
		const auto& foregroundItems = foregroundRoot->GetChildren();

		if (m_drawOrder < FOREGROUND_DRAW_ORDER)
		{
			for (Widget* frItem : foregroundItems)
			{
				if (frItem->GetFlags().IsSet(WF_FOREGROUND_BLOCKER))
				{
					m_isHovered = false;
					return;
				}
			}
		}

		const Vector2& pos = m_lgxWindow->GetMousePosition();
		m_isHovered		   = m_rect.IsPointInside(pos);

		if (m_parent && m_parent != m_manager->GetRoot() && m_parent != m_manager->GetForegroundRoot() && !m_parent->GetIsHovered())
			m_isHovered = false;
	}

	void Widget::CheckCustomTooltip()
	{
	}

	Widget* Widget::FindChildWithUserdata(void* ud)
	{
		for (Widget* c : m_children)
		{
			if (c->GetUserData() == ud)
				return c;

			Widget* found = c->FindChildWithUserdata(ud);

			if (found)
				return found;
		}

		return nullptr;
	}

	Widget* Widget::FindChildWithDebugName(const String& dbgName)
	{
		for (Widget* c : m_children)
		{
			if (c->GetWidgetProps().debugName.compare(dbgName) == 0)
				return c;

			Widget* found = c->FindChildWithDebugName(dbgName);

			if (found)
				return found;
		}

		return nullptr;
	}

	Widget* Widget::FindDeepestHovered()
	{
		for (Widget* c : m_children)
		{
			if (c->GetIsHovered())
				return c->FindDeepestHovered();
		}
		return this;
	}

	Vector2 Widget::GetStartFromMargins()
	{
		return m_rect.pos + Vector2(m_widgetProps.childMargins.left, m_widgetProps.childMargins.top);
	}

	Vector2 Widget::GetEndFromMargins()
	{
		return m_rect.GetEnd() - Vector2(m_widgetProps.childMargins.right, m_widgetProps.childMargins.bottom);
	}

	void Widget::SetIsDisabled(bool isDisabled)
	{
		m_isDisabled = isDisabled;

		for (auto* c : m_children)
			c->SetIsDisabled(isDisabled);
	}

	Vector2 Widget::GetMonitorSize()
	{
		return Vector2(static_cast<float>(m_lgxWindow->GetMonitorSize().x), static_cast<float>(m_lgxWindow->GetMonitorSize().y));
	}

	Vector2 Widget::GetWindowSize()
	{
		return Vector2(static_cast<float>(m_lgxWindow->GetSize().x), static_cast<float>(m_lgxWindow->GetSize().y));
	}

	Vector2 Widget::GetWindowPos()
	{
		return Vector2(static_cast<float>(m_lgxWindow->GetPosition().x), static_cast<float>(m_lgxWindow->GetPosition().y));
	}

	Rect Widget::GetTemporaryAlignedRect()
	{
		// When we haven't calculated our original rect from alignment just yet, but we still need to know the results immediately.
		const float posx  = GetParent()->GetPosX() + GetParent()->GetSizeX() * GetAlignedPosX();
		const float posy  = GetParent()->GetPosY() + GetParent()->GetSizeY() * GetAlignedPosY();
		const float sizex = GetFlags().IsSet(WF_USE_FIXED_SIZE_X) ? GetFixedSizeX() : GetParent()->GetSizeX() * GetAlignedSizeX();
		const float sizey = GetFlags().IsSet(WF_USE_FIXED_SIZE_Y) ? GetFixedSizeY() : GetParent()->GetSizeY() * GetAlignedSizeY();
		return Rect(Vector2(posx, posy), Vector2(sizex, sizey));
	}

	bool Widget::IsWidgetInHierarchy(Widget* widget)
	{
		if (UtilVector::Contains(m_children, widget))
			return true;

		for (auto* c : m_children)
		{
			if (c->IsWidgetInHierarchy(widget))
				return true;
		}

		return false;
	}

} // namespace Lina
