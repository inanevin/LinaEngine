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
	class Selectable : public Widget
	{
	public:
		Selectable() : Widget(WF_CONTROLLABLE){};
		virtual ~Selectable() = default;

		struct Properties
		{
			Color							  colorStart			  = Color(0.0f, 0.0f, 0.0f, 0.0f);
			Color							  colorEnd				  = Color(0.0f, 0.0f, 0.0f, 0.0f);
			Color							  colorLocalSelectedStart = Theme::GetDef().silent0;
			Color							  colorLocalSelectedEnd	  = Theme::GetDef().silent1;
			Color							  colorSelectedStart	  = Theme::GetDef().accentPrimary0;
			Color							  colorSelectedEnd		  = Theme::GetDef().accentPrimary1;
			Color							  colorOutline			  = Theme::GetDef().outlineColorBase;
			float							  rounding				  = 0.0f;
			float							  outlineThickness		  = 0.0f;
			Delegate<void(Selectable*, bool)> onSelectionChanged;
			Delegate<void(Selectable*)>		  onInteracted;
			Delegate<void(Selectable*)>		  onRightClick;
			Delegate<void()>				  onDockedOut;

			void SaveToStream(OStream& stream) const
			{
				colorStart.SaveToStream(stream);
				colorEnd.SaveToStream(stream);
				colorLocalSelectedStart.SaveToStream(stream);
				colorLocalSelectedEnd.SaveToStream(stream);
				colorSelectedStart.SaveToStream(stream);
				colorSelectedEnd.SaveToStream(stream);
				colorOutline.SaveToStream(stream);
				stream << rounding << outlineThickness;
			}

			void LoadFromStream(IStream& stream)
			{
				colorStart.LoadFromStream(stream);
				colorEnd.LoadFromStream(stream);
				colorLocalSelectedStart.LoadFromStream(stream);
				colorLocalSelectedEnd.LoadFromStream(stream);
				colorSelectedStart.LoadFromStream(stream);
				colorSelectedEnd.LoadFromStream(stream);
				colorOutline.LoadFromStream(stream);
				stream >> rounding >> outlineThickness;
			}
		};

		virtual void PreTick() override;
		virtual void Tick(float dt) override;
		virtual void Draw() override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;
		virtual bool OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act) override;

		inline Properties& GetProps()
		{
			return m_props;
		}

		inline virtual void SaveToStream(OStream& stream) const override
		{
			Widget::SaveToStream(stream);
			m_props.SaveToStream(stream);
		}

		inline virtual void LoadFromStream(IStream& stream) override
		{
			Widget::LoadFromStream(stream);
			m_props.LoadFromStream(stream);
		}

	private:
		static constexpr float COLOR_SPEED = 22.0f;

		Properties m_props		 = {};
		Color	   m_usedStart	 = Color(0.0f, 0.0f, 0.0f, 0.0f);
		Color	   m_usedEnd	 = Color(0.0f, 0.0f, 0.0f, 0.0f);
		bool	   m_wasSelected = false;
		bool	   m_dockedOut	 = false;
	};

	LINA_REFLECTWIDGET_BEGIN(Selectable)
	LINA_REFLECTWIDGET_END(Selectable)

} // namespace Lina
