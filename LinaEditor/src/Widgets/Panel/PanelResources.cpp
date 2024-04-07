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

#include "Editor/Widgets/Panel/PanelResources.hpp"
#include "Editor/Editor.hpp"
#include "Editor/IO/FileManager.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/System/System.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"

namespace Lina::Editor
{
	void PanelResources::Construct()
	{
		m_editor = m_system->CastSubsystem<Editor>(SubsystemType::Editor);

		Widget* base = m_manager->Allocate<Widget>("Base");
		base->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		base->SetAlignedPos(Vector2::Zero);
		base->SetAlignedSize(Vector2::One);
		AddChild(base);

		Widget* browser = m_manager->Allocate<Widget>("Browser");
		browser->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		browser->SetAlignedPos(Vector2::Zero);
		browser->SetAlignedSize(Vector2(0.25f, 1.0f));
		base->AddChild(browser);

		Widget* contents = m_manager->Allocate<Widget>("Contents");
		contents->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		contents->SetAlignedPos(Vector2(0.25f, 0.0f));
		contents->SetAlignedSize(Vector2(0.75f, 1.0f));
		base->AddChild(contents);
	}

	void PanelResources::Draw(int32 threadIndex)
	{
	}
} // namespace Lina::Editor
