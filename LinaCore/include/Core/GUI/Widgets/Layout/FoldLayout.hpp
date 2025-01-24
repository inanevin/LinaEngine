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
#include "Common/Tween/Tween.hpp"

namespace Lina
{
	class FoldLayout : public Widget
	{
	public:
		FoldLayout() : Widget(){};
		virtual ~FoldLayout() = default;

		struct Properties
		{
			Delegate<void(bool)> onFoldChanged;
			bool				 doubleClickChangesFold = true;
			bool				 lookForChevron			= true;
			float				 marginIncrease			= Theme::GetDef().baseIndentInner;
			bool				 useTween				= true;
			float				 tweenPower				= 6.0f;
			float				 tweenDuration			= 0.25f;

			void SaveToStream(OStream& stream) const
			{
				stream << doubleClickChangesFold << lookForChevron << marginIncrease << useTween << tweenPower << tweenDuration;
			}

			void LoadFromStream(IStream& stream)
			{
				stream >> doubleClickChangesFold >> lookForChevron >> marginIncrease >> useTween >> tweenPower >> tweenDuration;
			}
		};

		virtual void CalculateSize(float delta) override;
		virtual void Tick(float delta) override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;
		void		 SetIsUnfolded(bool unfolded);

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

		inline bool GetIsUnfolded() const
		{
			return m_unfolded;
		}

		inline Properties& GetProps()
		{
			return m_props;
		}

	private:
		static constexpr float FOLD_SPEED = 12.0f;
		LINA_REFLECTION_ACCESS(FoldLayout);
		Properties m_props	  = {};
		bool	   m_unfolded = false;
		Tween	   m_tween	  = {};
	};

	LINA_WIDGET_BEGIN(FoldLayout, Layout)
	LINA_FIELD(FoldLayout, m_props, "", FieldType::UserClass, GetTypeID<FoldLayout::Properties>())
	LINA_CLASS_END(FoldLayout)

	LINA_CLASS_BEGIN(FoldLayoutProperties)
	LINA_FIELD(FoldLayout::Properties, doubleClickChangesFold, "Toggle With Double Click", FieldType::Boolean, 0)
	LINA_FIELD(FoldLayout::Properties, lookForChevron, "Look For Chevron", FieldType::Boolean, 0)
	LINA_FIELD(FoldLayout::Properties, useTween, "Use Tween", FieldType::Boolean, 0)
	LINA_FIELD(FoldLayout::Properties, tweenPower, "Tween Power", FieldType::Float, 0)
	LINA_FIELD(FoldLayout::Properties, tweenDuration, "Tween Duration", FieldType::Float, 0)
	LINA_FIELD(FoldLayout::Properties, marginIncrease, "Margin Increase", FieldType::Float, 0)
	LINA_FIELD_DEPENDENCY(FoldLayout::Properties, tweenPower, "useTween", "1")
	LINA_FIELD_DEPENDENCY(FoldLayout::Properties, tweenDuration, "useTween", "1")
	LINA_CLASS_END(FoldLayoutProperties)
} // namespace Lina
