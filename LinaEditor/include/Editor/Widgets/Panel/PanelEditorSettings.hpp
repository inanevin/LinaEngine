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

#include "Editor/Widgets/Panel/Panel.hpp"
#include "Editor/CommonSettings.hpp"

namespace Lina
{
	class DirectionalLayout;
	class FoldLayout;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;

	class PanelEditorSettings : public Panel
	{
	public:
	public:
		PanelEditorSettings() : Panel(PanelType::EditorSettings) {};
		virtual ~PanelEditorSettings() = default;

		virtual void Construct() override;

		void SetSettingsInput(const EditorInputSettings& settings);

	private:
		void BuildSettingsInput();

	private:
		LINA_REFLECTION_ACCESS(PanelEditorSettings);

		Editor*				m_editor		   = nullptr;
		DirectionalLayout*	m_layout		   = nullptr;
		FoldLayout*			m_foldInput		   = nullptr;
		bool				m_foldValueInput   = true;
		EditorInputSettings m_settingsInput	   = {};
		EditorInputSettings m_oldSettingsInput = {};
	};

	LINA_WIDGET_BEGIN(PanelEditorSettings, Hidden)

	LINA_CLASS_END(PanelEditorSettings)

} // namespace Lina::Editor
