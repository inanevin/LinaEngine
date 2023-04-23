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

#ifndef GUIDrawerBase_HPP
#define GUIDrawerBase_HPP

#include "Graphics/Interfaces/IGUIDrawer.hpp"
#include "Core/StringID.hpp"
#include "Core/EditorCommon.hpp"
#include "Math/Rect.hpp"
#include "Serialization/ISerializable.hpp"

namespace Lina
{
	class Rect;
}
namespace Lina::Editor
{
	class GUINode;
	class Editor;
	class GUINodeDockArea;
	class GUINodePanel;
	class GUINodeDivider;
	class GUINodeDockPreview;

	class GUIDrawerBase : public IGUIDrawer, public ISerializable
	{
	public:
		GUIDrawerBase(Editor* editor, ISwapchain* swap);
		virtual ~GUIDrawerBase();

		virtual void OnKey(uint32 key, InputAction action) override;
		virtual void OnMouse(uint32 button, InputAction action) override;
		virtual void OnMousePos(const Vector2i& pos) override;
		virtual void OnMouseMove(const Vector2i& pos) override;
		virtual void OnMouseWheel(uint32 delta) override;
		virtual void OnLostFocus() override;
		virtual void OnWindowDrag(bool isDragging) override;
		virtual void OnDockAreasModified(){};

		void			 OnNodeDeleted(GUINode* node);
		void			 SetDockPreviewEnabled(bool enabled);
		GUINodeDockArea* SplitDockArea(GUINodeDockArea* area, DockSplitType type, const Vector<GUINodePanel*>& panels, float customSplit = 0.0f);
		GUINode*		 FindNode(StringID sid);
		void			 OnPayloadCreated(PayloadType type, void* data);
		bool			 OnPayloadDropped(PayloadType type, void* data);
		void			 RemoveDockArea(GUINodeDockArea* area);
		virtual void	 SaveToStream(OStream& stream) override;
		virtual void	 LoadFromStream(IStream& stream) override;

		inline GUINodeDockArea* GetFirstDockArea() const
		{
			return m_dockAreas[0];
		}

		inline GUINode* GetRoot() const
		{
			return m_root;
		}

		inline GUINodeDockPreview* GetDockPreview() const
		{
			return m_dockPreview;
		}

		inline const Vector<GUINodeDockArea*>& GetDockAreas() const
		{
			return m_dockAreas;
		}

		inline const Vector<GUINodeDivider*>& GetDividers() const
		{
			return m_dividers;
		}

		inline Editor* GetEditor() const
		{
			return m_editor;
		}

	protected:
		void DrawDockAreas(int threadID, const Rect& availableRect);

	private:
		GUINode* GetHovered(GUINode* parent);

	protected:
		GUINode* m_mouseDisablingNode = nullptr;
		GUINode* m_hoveredNode		  = nullptr;
		Editor*	 m_editor			  = nullptr;
		GUINode* m_root				  = nullptr;

		GUINodeDockPreview*		 m_dockPreview = nullptr;
		Vector<GUINodeDockArea*> m_dockAreas;
		Vector<GUINodeDivider*>	 m_dividers;
		uint32					 m_dockAreaCounter = 1;
		uint32					 m_dividerCounter  = 1;
	};
} // namespace Lina::Editor

#endif
