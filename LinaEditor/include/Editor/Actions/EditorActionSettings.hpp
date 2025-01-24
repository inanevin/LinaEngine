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

#include "EditorAction.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Editor/CommonSettings.hpp"

namespace Lina
{
	class Entity;
	class EntityWorld;
}; // namespace Lina

namespace Lina::Editor
{
	class Editor;

	class EditorActionSettingsPackaging : public EditorAction
	{
	public:
		EditorActionSettingsPackaging()			 = default;
		virtual ~EditorActionSettingsPackaging() = default;

		static EditorActionSettingsPackaging* Create(Editor* editor, const PackagingSettings& prevSettings, const PackagingSettings& currentSettings);
		virtual void						  Execute(Editor* editor, ExecType type) override;

	private:
		PackagingSettings m_prevSettings	= {};
		PackagingSettings m_currentSettings = {};
	};

	class EditorActionSettingsEditorInput : public EditorAction
	{
	public:
		EditorActionSettingsEditorInput()		   = default;
		virtual ~EditorActionSettingsEditorInput() = default;

		static EditorActionSettingsEditorInput* Create(Editor* editor, const EditorInputSettings& prevSettings, const EditorInputSettings& currentSettings);
		virtual void							Execute(Editor* editor, ExecType type) override;

	private:
		EditorInputSettings m_prevSettings	  = {};
		EditorInputSettings m_currentSettings = {};
	};

} // namespace Lina::Editor
