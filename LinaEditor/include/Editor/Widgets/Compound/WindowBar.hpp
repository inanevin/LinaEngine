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

#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Common/Serialization/StringSerialization.hpp"

namespace Lina::Editor
{
	class WindowBar : public DirectionalLayout
	{
	public:
		WindowBar()			 = default;
		virtual ~WindowBar() = default;

		struct BarProperties
		{
			bool   hasWindowButtons = false;
			bool   controlsDragRect = false;
			String title			= "";
			String icon				= "";

			void SaveToStream(OStream& stream) const
			{
				StringSerialization::SaveToStream(stream, title);
				StringSerialization::SaveToStream(stream, icon);
				stream << hasWindowButtons << controlsDragRect;
			}

			void LoadFromStream(IStream& stream)
			{
				StringSerialization::LoadFromStream(stream, title);
				StringSerialization::LoadFromStream(stream, icon);
				stream >> hasWindowButtons >> controlsDragRect;
			}
		};

		virtual void Construct() override;
		virtual void Initialize() override;
		virtual void PreTick() override;
		virtual void Tick(float delta) override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;

		void SetMinimizeDisabled(bool isDisabled);
		void SetMaximizeDisabled(bool isDisabled);
		void SetCloseDisabled(bool isDisabled);

		inline BarProperties& GetBarProps()
		{
			return m_barProps;
		}

		virtual void SaveToStream(OStream& stream) const override
		{
			Widget::SaveToStream(stream);
			m_props.SaveToStream(stream);
		}

		virtual void LoadFromStream(IStream& stream) override
		{
			Widget::LoadFromStream(stream);
			m_props.LoadFromStream(stream);
		}

	private:
		static constexpr float COLOR_SPEED = 15.0f;

	private:
		Rect			   m_dragRect	   = {};
		BarProperties	   m_barProps	   = {};
		DirectionalLayout* m_windowButtons = nullptr;
	};

	LINA_WIDGET_BEGIN(WindowBar, Editor)
	LINA_CLASS_END(WindowBar)

} // namespace Lina::Editor
