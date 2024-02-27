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

#include "Editor/Widgets/Docking/DockArea.hpp"
#include "Editor/Widgets/Docking/DockPreview.hpp"

namespace Lina::Editor
{
	void DockArea::Construct()
	{
	}

	void DockArea::Tick(float delta)
	{
		Widget::SetIsHovered();

		SetPos(m_parent->GetSize() * m_posAlign);
		SetSize(m_parent->GetSize() * m_sizeAlign);

		if (m_preview)
		{
			m_preview->SetSize(m_rect.size);
			m_preview->SetPos(m_rect.pos);
			m_preview->Tick(delta);
		}
	}

	void DockArea::Draw(int32 threadIndex)
	{
		if (m_preview)
			m_preview->Draw(threadIndex);
	}

	void DockArea::ShowPreview()
	{
		LINA_ASSERT(m_preview == nullptr, "");
		m_preview						= Allocate<DockPreview>("DockContainerPreview");
		m_preview->GetProps().isCentral = false;
		m_preview->Initialize();
	}

	void DockArea::HidePreview()
	{
		LINA_ASSERT(m_preview != nullptr, "");
		Deallocate(m_preview);
	}
} // namespace Lina::Editor
