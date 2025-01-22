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
#include "Common/Platform/LinaVGIncl.hpp"
#include "Core/Graphics/Resource/Font.hpp"

namespace Lina
{
	class Font;

	class Text : public Widget
	{
	public:
		Text() : Widget() {};
		virtual ~Text() = default;

		struct Properties
		{
			Delegate<void()>	  onClicked;
			String				  text						= "";
			ResourceID			  font						= Theme::GetDef().defaultFont;
			ColorGrad			  color						= Theme::GetDef().foreground0;
			LinaVG::TextAlignment alignment					= LinaVG::TextAlignment::Left;
			Vector4				  customClip				= Vector4::Zero;
			float				  textScale					= 1.0f;
			float				  wrapWidth					= 0.0f;
			bool				  isDynamic					= false;
			bool				  fetchCustomClipFromParent = false;
			bool				  fetchCustomClipFromSelf	= false;
			bool				  fetchWrapFromParent		= false;
			bool				  wordWrap					= true;
			bool				  delayOnClicked			= false;
			String*				  valuePtr					= nullptr;

			void SaveToStream(OStream& stream) const
			{
				stream << font << color << customClip;
				stream << text;
				stream << textScale << wrapWidth << isDynamic << fetchCustomClipFromSelf << fetchWrapFromParent << fetchWrapFromParent << wordWrap;
				stream << alignment;
			}

			void LoadFromStream(IStream& stream)
			{
				stream >> font >> color >> customClip;
				stream >> text;
				stream >> textScale >> wrapWidth >> isDynamic >> fetchCustomClipFromSelf >> fetchWrapFromParent >> fetchWrapFromParent >> wordWrap;
				stream >> alignment;
			}
		};

		virtual void Construct() override;
		virtual void Initialize() override;
		virtual void CalculateSize(float delta) override;
		virtual void Draw() override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;
		virtual void CollectResourceReferences(HashSet<ResourceID>& out) override;
		void		 UpdateTextAndCalcSize(const String& txt);
		void		 CalculateTextSize();

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
		LINA_REFLECTION_ACCESS(Text);

		Properties	  m_props					= {};
		float		  m_calculatedUIScaling		= 0.0f;
		LinaVG::Font* m_lvgFont					= nullptr;
		bool		  m_isSDF					= false;
		float		  m_lastPressSeconds		= 0.0f;
		bool		  m_waitingOnClickedDelay	= false;
		float		  m_lastCalculatedWrapWidth = 0.0f;
		ResourceID	  m_calculatedFont			= 0;
	};

	LINA_WIDGET_BEGIN(Text, Primitive)
	LINA_FIELD(Text, m_props, "Text Properties", FieldType::UserClass, GetTypeID<Text::Properties>());
	LINA_CLASS_END(Text)

	LINA_CLASS_BEGIN(TextProperties)
	LINA_FIELD(Text::Properties, text, "Text", FieldType::String, 0)
	LINA_FIELD(Text::Properties, font, "Font", FieldType::ResourceID, GetTypeID<Font>())
	LINA_FIELD(Text::Properties, alignment, "Alignment", FieldType::Enum, GetTypeID<LinaVG::TextAlignment>())
	LINA_FIELD(Text::Properties, textScale, "Scale", FieldType::Float, 0)
	LINA_FIELD(Text::Properties, wrapWidth, "Wrap Width", FieldType::Float, 0)
	LINA_FIELD(Text::Properties, wordWrap, "Word Wrap", FieldType::Boolean, 0)
	LINA_FIELD(Text::Properties, fetchWrapFromParent, "Fetch Wrap From Parent", FieldType::Boolean, 0)
	LINA_FIELD(Text::Properties, isDynamic, "Is Dynamic", FieldType::Boolean, 0)
	LINA_FIELD(Text::Properties, fetchCustomClipFromParent, "Clip From Parent", FieldType::Boolean, 0)
	LINA_FIELD(Text::Properties, fetchCustomClipFromSelf, "Clip From Self", FieldType::Boolean, 0)
	LINA_FIELD(Text::Properties, color, "Color", FieldType::ColorGrad, 0)
	LINA_CLASS_END(TextProperties)

} // namespace Lina
