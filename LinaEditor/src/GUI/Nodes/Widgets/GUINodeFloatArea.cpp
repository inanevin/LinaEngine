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

#include "GUI/Nodes/Widgets/GUINodeFloatArea.hpp"
#include "Math/Math.hpp"
#include "Data/CommonData.hpp"

namespace Lina::Editor
{
	GUINodeFloatArea::GUINodeFloatArea(GUIDrawerBase* drawer, int drawOrder) : GUINodeNumberArea(drawer, drawOrder)
	{
		m_lastDecimals = m_maxDecimals;
	}

	void GUINodeFloatArea::Draw(int threadID)
	{
		if (m_ptr != nullptr)
		{
			if (!Math::Equals(m_lastUpdatedValue, *m_ptr, 0.0001f))
				UpdateTitle(m_lastDecimals);
		}

		GUINodeNumberArea::Draw(threadID);
	}

	void GUINodeFloatArea::OnTitleChanged(const String& str)
	{
		try
		{
			const float floatVal = Internal::StringToFloat(str, m_lastDecimals);
			m_lastDecimals		 = Math::Min(m_maxDecimals, m_lastDecimals);

			if (m_ptr != nullptr)
				*m_ptr = floatVal;

			UpdateTitle(m_lastDecimals);
		}
		catch (Exception& e)
		{
			LINA_ERR("[GUINodeFloatArea] -> Exception! {0}", e.what());
			return;
		}
	}

	void GUINodeFloatArea::UpdateTitle(int decimals)
	{
		if (m_isEditing)
			return;

		if (m_ptr != nullptr)
		{
			const float myValue = *m_ptr;

			m_lastUpdatedValue = myValue;
			GUINodeTextArea::SetTitle(Internal::FloatToString(myValue, decimals));
		}
	}

	void GUINodeFloatArea::IncrementValue(const Vector2i& delta)
	{
		if (m_ptr == nullptr)
			return;

		*m_ptr = m_valOnIncrementStart + 0.1f * delta.x;
	}

	void GUINodeFloatArea::OnStartedIncrementing()
	{
		if (m_ptr == nullptr)
			return;

		m_valOnIncrementStart = *m_ptr;
	}

} // namespace Lina::Editor
