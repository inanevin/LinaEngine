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

#ifndef GUINodeFileMenu_HPP
#define GUINodeFileMenu_HPP

#include "GUI/Nodes/GUINode.hpp"
#include "Core/StringID.hpp"
#include "Data/Functional.hpp"
#include "Data/Vector.hpp"
#include "Data/String.hpp"
#include "Core/EditorCommon.hpp"

namespace Lina::Editor
{
	class GUINodeLayoutHorizontal;
	class GUINodeLayoutVertical;
	class GUINodeButton;

	enum class FMPopupElementType
	{
		Default,
		Divider,
		Toggle,
		Expandable
	};

	class GUINodeFMPopupElement : public GUINode
	{
	public:
		GUINodeFMPopupElement(GUIDrawerBase* drawer, int drawOrder, FMPopupElementType type) : m_type(type), GUINode(drawer, drawOrder){};
		virtual ~GUINodeFMPopupElement() = default;

		virtual void  Draw(int threadID) override;
		virtual void  OnClicked(uint32 button) override;
		virtual float GetTotalWidth();

		inline FMPopupElementType GetType() const
		{
			return m_type;
		}

		inline void SetIsLastHovered(bool isLastHovered)
		{
			m_isLastHovered = isLastHovered;
		}

		inline void SetShortcut(Shortcut sc)
		{
			m_shortcut = sc;
		}

	protected:
		float			   m_shortcutXStartRight = 0.0f;
		Shortcut		   m_shortcut			 = Shortcut::None;
		bool			   m_isLastHovered		 = false;
		FMPopupElementType m_type				 = FMPopupElementType::Default;
	};

	class GUINodeFMPopupElementDivider : public GUINodeFMPopupElement
	{
	public:
		GUINodeFMPopupElementDivider(GUIDrawerBase* drawer, int drawOrder) : GUINodeFMPopupElement(drawer, drawOrder, FMPopupElementType::Divider){};
		virtual ~GUINodeFMPopupElementDivider() = default;
		virtual void  Draw(int threadID) override;
		virtual void  OnClicked(uint32 button) override{};
		virtual float GetTotalWidth() override;
	};

	class GUINodeFMPopupElementToggle : public GUINodeFMPopupElement
	{
	public:
		GUINodeFMPopupElementToggle(GUIDrawerBase* drawer, int drawOrder) : GUINodeFMPopupElement(drawer, drawOrder, FMPopupElementType::Toggle){};
		virtual ~GUINodeFMPopupElementToggle() = default;
		virtual void  Draw(int threadID) override;
		virtual void  OnClicked(uint32 button) override;
		virtual float GetTotalWidth() override;

		inline void SetValue(bool val)
		{
			m_value = val;
		}

	protected:
		bool m_value = false;
	};

	class GUINodeFMPopup;
	class GUINodeFMPopupElementExpandable : public GUINodeFMPopupElement
	{
	public:
		GUINodeFMPopupElementExpandable(GUIDrawerBase* drawer, int drawOrder) : GUINodeFMPopupElement(drawer, drawOrder, FMPopupElementType::Expandable){};
		virtual ~GUINodeFMPopupElementExpandable() = default;
		virtual void  Draw(int threadID) override;
		virtual void  OnClicked(uint32 button) override{};
		virtual float GetTotalWidth() override;

		inline void SetExpandedPopup(GUINodeFMPopup* popup)
		{
			m_popup = popup;
		}

	protected:
		GUINodeFMPopup* m_popup = nullptr;
	};

	class GUINodeFMPopup : public GUINode
	{
	public:
		GUINodeFMPopup(GUIDrawerBase* drawer, int drawOrder);
		virtual ~GUINodeFMPopup() = default;

		virtual void					 Draw(int threadID) override;
		GUINodeFMPopupElementDivider*	 AddDivider(const char* title);
		GUINodeFMPopupElement*			 AddDefault(const char* title);
		GUINodeFMPopupElementToggle*	 AddToggle(const char* title, bool initialValue);
		GUINodeFMPopupElementExpandable* AddExpandable(const char* title, GUINodeFMPopup* popup);

		inline GUINodeLayoutVertical* GetLayout()
		{
			return m_layout;
		}

		inline Vector<GUINodeFMPopupElement*>& GetElements()
		{
			return m_elements;
		}
		inline void SwitchedFrom()
		{
			m_lastHoveredElement = nullptr;
		}

	protected:
		void OnPressedItem(GUINode* node);

	protected:
		GUINodeFMPopupElement*		   m_lastHoveredElement = nullptr;
		Vector<GUINodeFMPopupElement*> m_elements;
		GUINodeLayoutVertical*		   m_layout = nullptr;
	};

	class GUINodeFileMenu : public GUINode
	{
	public:
		GUINodeFileMenu(GUIDrawerBase* drawer, int drawOrder);

		virtual ~GUINodeFileMenu() = default;

		virtual void Draw(int threadID) override;
		virtual bool OnMouse(uint32 button, InputAction act) override;

		template <typename T, typename... Popups> void AddPopup(T& popup, Popups&&... rest)
		{
			AddSinglePopup(popup);
			AddPopup(rest...);
		}

		void AddPopup()
		{
		}

	protected:
		void AddSinglePopup(GUINodeFMPopup* popup);
		void OnButtonClicked(GUINode* but);
		void OnPressedPopupElement(GUINode* node);
		void ResetTargets();

	private:
		GUINodeFMPopup*			 m_targetPopup	= nullptr;
		GUINodeButton*			 m_targetButton = nullptr;
		GUINodeLayoutHorizontal* m_layout		= nullptr;
		Vector<GUINodeButton*>	 m_buttons;
	};
} // namespace Lina::Editor

#endif
