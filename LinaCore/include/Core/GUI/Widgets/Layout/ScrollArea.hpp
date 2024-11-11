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

#pragma once

#include "Core/GUI/Widgets/Widget.hpp"

namespace Lina
{
	class ScrollArea : public Widget
	{
	public:
		ScrollArea() : Widget(WF_TICK_AFTER_CHILDREN){};
		virtual ~ScrollArea() = default;

		struct Properties
		{
			DirectionOrientation direction			  = DirectionOrientation::Horizontal;
			Color				 colorBarBackground	  = Theme::GetDef().background2;
			ColorGrad			 colorBar			  = {Theme::GetDef().accentPrimary1, Theme::GetDef().accentPrimary0};
			Color				 colorHovered		  = Theme::GetDef().accentPrimary2;
			Color				 colorPressed		  = Theme::GetDef().accentPrimary1;
			float				 barRounding		  = Theme::GetDef().baseRounding * 2;
			float				 barThickness		  = Theme::GetDef().baseItemHeight / 2;
			float				 mouseWheelMultiplier = 0.05f;

			void SaveToStream(OStream& stream) const
			{
				stream << direction;
				stream << colorBarBackground << colorBar << colorHovered << colorPressed;
				stream << barRounding << barThickness;
			}

			void LoadFromStream(IStream& stream)
			{
				stream >> direction;
				stream >> colorBarBackground >> colorBar >> colorHovered >> colorPressed;
				stream >> barRounding >> barThickness;
			}
		};

		virtual void Tick(float delta) override;
		virtual void Draw() override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;
		virtual bool OnMouseWheel(float amt) override;
		void		 ScrollToChild(Widget* w);
		bool		 IsBarHovered() const;
		void		 ScrollToStart();
		void		 ScrollToEnd();
		void		 CheckScroll();
		bool		 IsScrollAtEnd() const;

		virtual void SaveToStream(OStream& stream) const override
		{
			Widget::SaveToStream(stream);
			stream << m_props;
		}

		virtual void LoadFromStream(IStream& stream) override
		{
			Widget::LoadFromStream(stream);
			stream >> m_props;
		}

		inline Properties& GetProps()
		{
			return m_props;
		}

		inline void SetTarget(Widget* target)
		{
			m_targetWidget = target;
		}

		inline bool GetIsBarVisible() const
		{
			return m_barVisible && m_canDrawBar;
		}

		inline void SetCanDrawBar(bool canDraw)
		{
			m_canDrawBar = canDraw;
		}

	private:
	private:
		static constexpr float SCROLL_SMOOTH = 15.0f;
		LINA_REFLECTION_ACCESS(ScrollArea);

		Properties m_props				  = {};
		bool	   m_canDrawBar			  = true;
		float	   m_scrollAmount		  = 0.0f;
		float	   m_minScroll			  = 0.0f;
		float	   m_maxScroll			  = 0.0f;
		float	   m_sizeToChildSizeRatio = 0.0f;
		float	   m_pressDelta			  = 0.0f;
		float	   m_totalChildSize		  = 0.0f;
		Widget*	   m_targetWidget		  = nullptr;
		bool	   m_barVisible			  = false;
		bool	   m_barHovered			  = false;
		Rect	   m_barBGRect			  = {};
		Rect	   m_barRect			  = {};
		Vector2	   m_start				  = {};
		Vector2	   m_end				  = {};
		Vector2	   m_sz					  = {};
	};

	LINA_WIDGET_BEGIN(ScrollArea, Layout)
	LINA_FIELD(ScrollArea, m_props, "", FieldType::UserClass, GetTypeID<ScrollArea::Properties>())
	LINA_CLASS_END(ScrollArea)

	LINA_CLASS_BEGIN(ScrollAreaProperties)
	LINA_FIELD(ScrollArea::Properties, direction, "Direction", FieldType::Enum, GetTypeID<DirectionOrientation>())
	LINA_FIELD(ScrollArea::Properties, colorBarBackground, "Bar Background", FieldType::Color, 0)
	LINA_FIELD(ScrollArea::Properties, colorBar, "Bar Color", FieldType::Color, 0)
	LINA_FIELD(ScrollArea::Properties, colorHovered, "Bar Color Hovered", FieldType::Color, 0)
	LINA_FIELD(ScrollArea::Properties, colorPressed, "Bar Color Pressed", FieldType::Color, 0)
	LINA_FIELD(ScrollArea::Properties, barRounding, "Bar Rounding", FieldType::Float, 0)
	LINA_FIELD(ScrollArea::Properties, barThickness, "Bar Thickness", FieldType::Float, 0)
	LINA_CLASS_END(ScrollAreaProperties)
} // namespace Lina
