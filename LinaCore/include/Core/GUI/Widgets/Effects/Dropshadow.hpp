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
#include "Common/Serialization/VectorSerialization.hpp"

namespace Lina
{
	class Dropshadow : public Widget
	{
	public:
		Dropshadow() : Widget(0)
		{
		}
		virtual ~Dropshadow() = default;

		struct Properties
		{
			Color		color	  = Theme::GetDef().background0;
			Direction	direction = Direction::Center;
			uint32		steps	  = 1;
			float		thickness = Theme::GetDef().baseOutlineThickness;
			float		rounding  = Theme::GetDef().baseRounding;
			Vector<int> onlyRoundCorners;

			void SaveToStream(OStream& stream) const
			{
				color.SaveToStream(stream);
				stream << steps << thickness << rounding << static_cast<uint8>(direction);
				VectorSerialization::SaveToStream_PT(stream, onlyRoundCorners);
			}

			void LoadFromStream(IStream& stream)
			{
				color.LoadFromStream(stream);
				stream >> steps >> thickness >> rounding;
				uint8 dir = 0;
				stream >> dir;
				direction = static_cast<Direction>(dir);
				VectorSerialization::LoadFromStream_PT(stream, onlyRoundCorners);
			}
		};

		virtual void Draw() override;

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
		Properties m_props = {};
	};

	LINA_REFLECTWIDGET_BEGIN(Dropshadow)
	LINA_REFLECTWIDGET_END(Dropshadow)

} // namespace Lina
