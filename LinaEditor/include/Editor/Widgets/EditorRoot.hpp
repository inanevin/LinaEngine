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

#include "Core/GUI/Widgets/Compound/FileMenu.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"

namespace Lina
{
	class Icon;
	class FileMenu;
	class Text;
	class Icon;
	class Texture;
} // namespace Lina

namespace Lina::Editor
{
	class DockArea;
	class Editor;

	class EditorRoot : public DirectionalLayout, public FileMenuListener
	{
	public:
		EditorRoot()		  = default;
		virtual ~EditorRoot() = default;

		virtual void Construct() override;
		virtual void Tick(float delta) override;
		virtual void Draw(int32 threadIndex) override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;
		virtual bool OnItemClicked(StringID sid) override;
		virtual void OnGetItemData(StringID sid, Vector<FileMenuItem::Data>& outData) override;
		virtual bool IsItemDisabled(StringID sid) override;
		void		 SetProjectName(const String& name);

		inline Widget* GetPanelArea() const
		{
			return m_panelArea;
		}

	private:
		static constexpr float COLOR_SPEED = 15.0f;

		Rect			   m_dragRect		 = {};
		FileMenu*		   m_fileMenu		 = nullptr;
		Widget*			   m_windowButtons	 = nullptr;
		Texture*		   m_titleImage		 = nullptr;
		Text*			   m_projectNameText = nullptr;
		Widget*			   m_panelArea		 = nullptr;
		DirectionalLayout* m_titleBar		 = nullptr;
		Editor*			   m_editor			 = nullptr;
		Icon*			   m_saveIcon		 = nullptr;
	};

} // namespace Lina::Editor
