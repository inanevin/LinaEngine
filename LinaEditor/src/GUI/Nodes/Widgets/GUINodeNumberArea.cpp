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

#include "GUI/Nodes/Widgets/GUINodeNumberArea.hpp"
#include "Math/Math.hpp"
#include "Data/CommonData.hpp"

namespace Lina::Editor
{
	GUINodeNumberArea::GUINodeNumberArea(GUIDrawerBase* drawer, int drawOrder) : GUINodeTextArea(drawer, drawOrder)
	{
		m_inputMask.Set(CharacterMask::Number | CharacterMask::Separator);
	}

	void GUINodeNumberArea::SetValue(float value, int decimals)
	{
		value = Math::Clamp(value, m_minValue, m_maxValue);

		if (m_isInteger)
		{
			const int intVal = static_cast<int>(value);

			GUINodeTextArea::SetTitle(TO_STRING(intVal));

			if (m_intVar != nullptr)
				*m_intVar = intVal;
		}
		else
		{
			GUINodeTextArea::SetTitle(Internal::FloatToString(value, decimals));

			if (m_floatVar != nullptr)
				*m_floatVar = value;
		}
	}

	void GUINodeNumberArea::OnTitleChanged(const String& str)
	{
		if (m_isInteger)
		{
			try
			{
				const int intVal = Internal::StringToInt(str);
				SetValue(static_cast<float>(intVal), 0);
			}
			catch (Exception& e)
			{
				LINA_ERR("[GUINodeNumberArea] -> Exception! {0}", e.what());
				return;
			}
		}
		else
		{
			try
			{
				int			decimals = 0;
				const float floatVal = Internal::StringToFloat(str, decimals);
				SetValue(floatVal, Math::Min(m_maxDecimals, decimals));
			}
			catch (Exception& e)
			{
				LINA_ERR("[GUINodeNumberArea] -> Exception! {0}", e.what());
				return;
			}
		}
	}

	bool GUINodeNumberArea::VerifyTitle()
	{
		const bool passes = GUINodeTextArea::VerifyTitle();

		if (!passes)
			return false;

		m_title = Internal::FixStringNumber(m_title);

		return true;
	}

} // namespace Lina::Editor
