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

#ifndef GUINodeSelection_HPP
#define GUINodeSelection_HPP

#include "GUI/Nodes/GUINode.hpp"
#include "Data/String.hpp"
#include "Data/Vector.hpp"

namespace Lina::Editor
{
	class GUINodeSelection : public GUINode
	{

	protected:
		struct ReorderRequest
		{
			int				  state	 = 0;
			GUINodeSelection* src	 = nullptr;
			GUINodeSelection* target = nullptr;
		};

	public:
		GUINodeSelection(GUIDrawerBase* drawer, int drawOrder);
		virtual ~GUINodeSelection() = default;

		virtual void	Draw(int threadID) override;
		virtual Vector2 CalculateSize() override;
		virtual void	OnPressed(uint32 button) override;
		virtual void	OnDoubleClicked() override;
		virtual void	OnPayloadAccepted() override;
		void			AddChildSelection(GUINodeSelection* sel);
		void			RemoveChildSelection(GUINodeSelection* sel);
		void			PlaceChildAfter(GUINodeSelection* child, GUINodeSelection* placeAfter);
		void			PlaceChildBefore(GUINodeSelection* child, GUINodeSelection* placeBefore);

		inline void SetXOffset(float off)
		{
			m_xOffset = off;
		}

		inline void SetSelected(bool selected)
		{
			m_isSelected = selected;
		}

		inline void SetCallbackDetached(Delegate<void(GUINodeSelection*, const Vector2& delta)>&& onDetached)
		{
			m_onDetached = onDetached;
		}

		inline const Vector2& GetLastCalculatedTextSize() const
		{
			return m_lastCalculatedTextSize;
		}

		inline bool GetIsExpanded() const
		{
			return m_isExpanded;
		}

		inline void SetIsExpanded(bool isExpanded)
		{
			m_isExpanded		  = isExpanded;
			m_caretRotationTarget = m_isExpanded ? 90.0f : 0.0f;
		}

	protected:
		virtual void	DrawBackground(int threadID);
		virtual Vector2 DrawIcons(int threadID, float iconsStart);
		virtual void	DrawTitleText(int threadID, float textStart);
		virtual void	HandleDrag(int threadID);
		virtual void	HandlePayload(int threadID);

	protected:
		ReorderRequest									  m_currentReorderRequest;
		Delegate<void(GUINodeSelection*, const Vector2&)> m_onDetached;
		Vector<GUINodeSelection*>						  m_childSelections;
		Vector2											  m_lastCalculatedTextSize = Vector2::Zero;
		Rect											  m_caretRect			   = Rect();
		Rect											  m_ownRect				   = Rect();
		float											  m_caretRotationTarget	   = 0.0f;
		bool											  m_isSelected			   = false;
		bool											  m_isExpanded			   = false;
		float											  m_xOffset				   = 0.0f;
		int												  m_payloadDropState	   = 0;
	};

} // namespace Lina::Editor

#endif
