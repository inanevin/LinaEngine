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

#include "Editor/Widgets/Panel/PanelAudioViewer.hpp"
#include "Editor/Editor.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"

namespace Lina::Editor
{

	void PanelAudioViewer::Construct()
	{
		PanelResourceViewer::Construct();
	}

	void PanelAudioViewer::Initialize()
	{
		PanelResourceViewer::Initialize();
		if (!m_resource)
			return;

		UpdateAudioProps();
	}

	void PanelAudioViewer::OnGeneralMetaChanged(const MetaType& meta, FieldBase* field)
	{
	}

	void PanelAudioViewer::OnResourceMetaChanged(const MetaType& meta, FieldBase* field)
	{
		// Application::GetLGX()->Join();
		// RegenGPU();
	}

	void PanelAudioViewer::RegenHW()
	{
		Audio* aud = static_cast<Audio*>(m_resource);
	}

	void PanelAudioViewer::UpdateAudioProps()
	{
		m_audioName = m_resource->GetName();
	}

} // namespace Lina::Editor