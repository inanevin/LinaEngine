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
#include "Common/Data/String.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina
{
	class Font;

	class Icon : public Widget
	{
	public:
		Icon() : Widget()
		{
		}
		virtual ~Icon() = default;

		struct Properties
		{
			Delegate<void()> onClicked;
			String			 icon		= "";
			String			 iconAlt	= "";
			ResourceID		 font		= Theme::GetDef().iconFont;
			ColorGrad		 color		= Theme::GetDef().foreground0;
			float			 textScale	= 0.5f;
			bool			 isDynamic	= true;
			bool			 useAltIcon = false;

			bool  dynamicSizeToParent = false;
			float dynamicSizeScale	  = 1.0f;

			void SaveToStream(OStream& stream) const
			{
				stream << color;
				stream << icon;
				stream << font << textScale << isDynamic;
				stream << dynamicSizeToParent << dynamicSizeScale;
			}

			void LoadFromStream(IStream& stream)
			{
				stream >> color;
				stream >> icon;
				stream >> font >> textScale >> isDynamic;
				stream >> dynamicSizeToParent >> dynamicSizeScale;
			}
		};

		virtual void Construct() override;
		virtual void Initialize() override;
		virtual void CalculateSize(float dt) override;
		virtual void Draw() override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;
		virtual void CollectResourceReferences(HashSet<ResourceID>& out) override;

		void CalculateIconSize();

		inline Properties& GetProps()
		{
			return m_props;
		}

		inline LinaVG::Font* GetLVGFont()
		{
			return m_lvgFont;
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
		LINA_REFLECTION_ACCESS(Icon);

		Properties			m_props				  = {};
		LinaVG::TextOptions m_textOptions		  = {};
		float				m_calculatedUIScaling = 0.0f;
		LinaVG::Font*		m_lvgFont			  = nullptr;
		float				m_lastDynScale		  = 0.0f;
	};

	LINA_WIDGET_BEGIN(Icon, Primitive)
	LINA_FIELD(Icon, m_props, "", FieldType::UserClass, GetTypeID<Icon::Properties>());
	LINA_CLASS_END(Icon)

	LINA_CLASS_BEGIN(IconProperties)
	LINA_FIELD(Icon::Properties, icon, "Icon", FieldType::String, 0)
	LINA_FIELD(Icon::Properties, font, "Font", FieldType::ResourceID, GetTypeID<Font>())
	LINA_FIELD(Icon::Properties, textScale, "Scale", FieldType::Float, 0)
	LINA_FIELD(Icon::Properties, isDynamic, "Dynamic", FieldType::Boolean, 0)
	LINA_FIELD(Icon::Properties, color, "Color", FieldType::ColorGrad, 0)
	LINA_FIELD(Icon::Properties, dynamicSizeToParent, "Dynamic Size To Parent", FieldType::Boolean, 0)
	LINA_FIELD(Icon::Properties, dynamicSizeScale, "Scale", FieldType::Float, 0)

	LINA_FIELD_DEPENDENCY(Icon::Properties, dynamicSizeScale, "dynamicSizeToParent", "1")
	LINA_CLASS_END(IconProperties)

} // namespace Lina
