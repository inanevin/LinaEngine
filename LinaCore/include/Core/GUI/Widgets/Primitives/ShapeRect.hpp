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
	class Font;
	class Texture;
	class TextureAtlasImage;

	class ShapeRect : public Widget
	{
	public:
		ShapeRect()			 = default;
		virtual ~ShapeRect() = default;

		struct Properties
		{
			Color colorStart   = Theme::GetDef().background0;
			Color colorEnd	   = Theme::GetDef().background0;
			Color colorOutline = Theme::GetDef().outlineColorBase;

			Vector<int>		   onlyRoundCorners;
			float			   rounding			 = 0.0f;
			float			   outlineThickness	 = 0.0f;
			Texture*		   imageTexture		 = nullptr;
			TextureAtlasImage* imageTextureAtlas = nullptr;
			bool			   fitImage			 = false;

			Delegate<void()> onClicked;

			void SaveToStream(OStream& stream) const
			{
				colorStart.SaveToStream(stream);
				colorEnd.SaveToStream(stream);
				colorOutline.SaveToStream(stream);
				VectorSerialization::SaveToStream_PT(stream, onlyRoundCorners);
				stream << rounding << outlineThickness << fitImage;
			}

			void LoadFromStream(IStream& stream)
			{
				colorStart.LoadFromStream(stream);
				colorEnd.LoadFromStream(stream);
				colorOutline.LoadFromStream(stream);
				VectorSerialization::LoadFromStream_PT(stream, onlyRoundCorners);
				stream >> rounding >> outlineThickness >> fitImage;
			}
		};

		virtual void Draw() override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction action) override;

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
		Properties m_props	   = {};
		Color	   m_usedColor = Theme::GetDef().background0;
	};

	LINA_REFLECTWIDGET_BEGIN(ShapeRect)
	LINA_REFLECTWIDGET_END(ShapeRect)

} // namespace Lina
