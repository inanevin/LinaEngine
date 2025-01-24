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

#include "Editor/Actions/EditorActionSettings.hpp"
#include "Editor/Widgets/Panel/PanelProjectSettings.hpp"
#include "Editor/Widgets/Panel/PanelEditorSettings.hpp"
#include "Editor/Editor.hpp"

namespace Lina::Editor
{

	EditorActionSettingsPackaging* EditorActionSettingsPackaging::Create(Editor* editor, const PackagingSettings& prevOptions, const PackagingSettings& currentOptions)
	{
		EditorActionSettingsPackaging* action = new EditorActionSettingsPackaging();
		action->m_prevSettings				  = prevOptions;
		action->m_currentSettings			  = currentOptions;
		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}

	void EditorActionSettingsPackaging::Execute(Editor* editor, ExecType type)
	{
		Panel* panel = editor->GetWindowPanelManager().FindPanelOfType(PanelType::ProjectSettings, 0);
		if (!panel)
			return;

		if (type == ExecType::Undo)
		{
			static_cast<PanelProjectSettings*>(panel)->SetSettingsPackaging(m_prevSettings);
		}
		else if (type == ExecType::Redo)
		{
			static_cast<PanelProjectSettings*>(panel)->SetSettingsPackaging(m_currentSettings);
		}
	}
	EditorActionSettingsEditorInput* EditorActionSettingsEditorInput::Create(Editor* editor, const EditorInputSettings& prevSettings, const EditorInputSettings& currentSettings)
	{
		EditorActionSettingsEditorInput* action = new EditorActionSettingsEditorInput();
		action->m_prevSettings					= prevSettings;
		action->m_currentSettings				= currentSettings;
		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}
	void EditorActionSettingsEditorInput::Execute(Editor* editor, ExecType type)
	{
		Panel* panel = editor->GetWindowPanelManager().FindPanelOfType(PanelType::EditorSettings, 0);
		if (!panel)
			return;

		if (type == ExecType::Undo)
		{
			static_cast<PanelEditorSettings*>(panel)->SetSettingsInput(m_prevSettings);
		}
		else if (type == ExecType::Redo)
		{
			static_cast<PanelEditorSettings*>(panel)->SetSettingsInput(m_currentSettings);
		}
	}
} // namespace Lina::Editor
