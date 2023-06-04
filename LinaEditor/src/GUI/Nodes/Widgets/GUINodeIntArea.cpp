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

#include "GUI/Nodes/Widgets/GUINodeIntArea.hpp"
#include "Math/Math.hpp"
#include "Data/CommonData.hpp"

namespace Lina::Editor
{
	GUINodeIntArea::GUINodeIntArea(GUIDrawerBase* drawer, int drawOrder) : GUINodeNumberArea(drawer, drawOrder)
	{
	}

	void GUINodeIntArea::Draw(int threadID)
	{
		if (m_ptr != nullptr)
		{
			if (m_lastUpdatedValue != *m_ptr)
				UpdateTitle(0);
		}
		GUINodeNumberArea::Draw(threadID);
	}

	void GUINodeIntArea::OnTitleChanged(const String& str)
	{
		try
		{
			const int intVal = Internal::StringToInt(str);

			if (m_ptr != nullptr)
				*m_ptr = intVal;

			UpdateTitle(0);
		}
		catch (Exception& e)
		{
			LINA_ERR("[GUINodeIntArea] -> Exception! {0}", e.what());
			return;
		}
	}

	void GUINodeIntArea::UpdateTitle(int decimals)
	{
		if (m_isEditing)
			return;

		if (m_ptr != nullptr)
		{
			const int myValue = *m_ptr;

			m_lastUpdatedValue = myValue;

			GUINodeTextArea::SetTitle(TO_STRING(myValue));
		}
	}

	void GUINodeIntArea::IncrementValue(const Vector2i& delta)
	{
		if (m_ptr == nullptr)
			return;

		*m_ptr = m_valOnIncrementStart + delta.x;
	}

	void GUINodeIntArea::OnStartedIncrementing()
	{
		if (m_ptr == nullptr)
			return;

		m_valOnIncrementStart = *m_ptr;
	}

} // namespace Lina::Editor
