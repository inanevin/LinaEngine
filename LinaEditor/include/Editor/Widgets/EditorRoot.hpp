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
#include "Common/Data/Mutex.hpp"
#include "Editor/Project/ProjectManager.hpp"

namespace Lina
{
	class Icon;
	class FileMenu;
	class Text;
	class Icon;
	class Texture;
	class EntityWorld;
} // namespace Lina

namespace Lina::Editor
{
	class DockArea;
	class Editor;

	class EditorRoot : public DirectionalLayout, public FileMenuListener, public ProjectManagerListener
	{
	public:
		EditorRoot()		  = default;
		virtual ~EditorRoot() = default;

		virtual void Construct() override;
		virtual void Destruct() override;
		virtual void PreTick() override;
		virtual void Tick(float delta) override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;
		virtual bool OnFileMenuItemClicked(FileMenu* filemenu, StringID sid, void* userData) override;
		virtual void OnFileMenuGetItems(FileMenu* filemenu, StringID sid, Vector<FileMenuItem::Data>& outData, void* userData) override;
		virtual void OnProjectOpened(ProjectData* project) override;
		void		 SetIsPlaying(bool isPlaying);

		inline Widget* GetPanelArea() const
		{
			return m_panelArea;
		}

	private:
		static constexpr float COLOR_SPEED = 15.0f;

		Rect			   m_dragRect		 = {};
		FileMenu*		   m_fileMenu		 = nullptr;
		Widget*			   m_windowButtons	 = nullptr;
		Text*			   m_projectNameText = nullptr;
		Widget*			   m_panelArea		 = nullptr;
		DirectionalLayout* m_titleBar		 = nullptr;
		Icon*			   m_linaIcon		 = nullptr;
		EntityWorld*	   m_currentWorld	 = nullptr;
		bool			   m_isPlaying		 = false;
	};

	LINA_WIDGET_BEGIN(EditorRoot, Hidden)
	LINA_CLASS_END(EditorRoot)

} // namespace Lina::Editor
