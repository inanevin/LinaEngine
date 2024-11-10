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
#include "Common/Data/Streams.hpp"

namespace Lina
{
	class Resource;
	class Button;
	class FoldLayout;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;

	class PanelResourceViewer : public Panel
	{
	public:
		PanelResourceViewer() = default;
		PanelResourceViewer(PanelType type, TypeID resourceTID, TypeID panelTID, uint32 flags = 0) : Panel(type, flags), m_resourceTID(resourceTID), m_panelTID(panelTID){};
		virtual ~PanelResourceViewer() = default;

		virtual void Construct() override;
		virtual void Destruct() override;
		virtual void Initialize() override;

	protected:
		Widget* BuildButtonLayout();
		void	SaveResource();
		void	ReimportResource();
		void	SetRuntimeDirty(bool runtimeDirty);
		Button* BuildButton(const String& title, const String& icon);
		void	DisableRecursively(Widget* parent);

	protected:
		Resource* m_resource	   = nullptr;
		Widget*	  m_resourceBG	   = nullptr;
		Widget*	  m_inspector	   = nullptr;
		Editor*	  m_editor		   = nullptr;
		TypeID	  m_resourceTID	   = 0;
		TypeID	  m_panelTID	   = 0;
		Button*	  m_saveButton	   = nullptr;
		Button*	  m_reimportButton = nullptr;
		bool	  m_previewOnly	   = false;
		bool	  m_runtimeDirty   = false;
		uint64	  m_resourceSpace  = 0;
	};

} // namespace Lina::Editor
