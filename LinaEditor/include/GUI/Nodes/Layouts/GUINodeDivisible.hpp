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

#ifndef GUINodeDivisible_HPP
#define GUINodeDivisible_HPP

#include "GUI/Nodes/GUINode.hpp"
#include "Core/EditorCommon.hpp"

namespace Lina::Editor
{
	class GUINodeDivider;

	class GUINodeDivisible : public GUINode
	{
	public:
		GUINodeDivisible(GUIDrawerBase* drawer, int drawOrder) : GUINode(drawer, drawOrder){};
		virtual ~GUINodeDivisible() = default;

		virtual void	Draw(int threadID) override;
		void			AddSize(float delta, Direction dir);
		bool			CanAddPos(float delta, Direction dir);
		bool			CanSubtractSize(float delta, Direction dir);
		void			AddPos(float delta, Direction dir);
		void			SubtractSize(float delta, Direction dir);
		void			SubtractPos(float delta, Direction dir);
		GUINodeDivider* FindDividerToRemove();
		void			FindDividersFromLoadedData(const Vector<GUINodeDivider*>& dividers);
		virtual void	SaveToStream(OStream& stream) override;
		virtual void	LoadFromStream(IStream& stream) override;

		const Rect& GetSplitRect() const
		{
			return m_splitRect;
		}

		void SetSplitRect(const Rect& r)
		{
			m_splitRect = r;
		}

		void SetTotalAvailableRect(const Rect& r)
		{
			m_totalAvailableRect = r;
		}

		inline GUINodeDivider* GetDivider(DockSplitType split) const
		{
			switch (split)
			{
			case DockSplitType::Left:
				return m_dividerLeft;
			case DockSplitType::Right:
				return m_dividerRight;
			case DockSplitType::Up:
				return m_dividerUp;
			case DockSplitType::Down:
				return m_dividerDown;
			default:
				return nullptr;
			}
			return nullptr;
		}

		inline void SetDivider(DockSplitType split, GUINodeDivider* divider)
		{
			switch (split)
			{
			case DockSplitType::Left:
				m_dividerLeft = divider;
				break;
			case DockSplitType::Right:
				m_dividerRight = divider;
				break;
			case DockSplitType::Up:
				m_dividerUp = divider;
				break;
			case DockSplitType::Down:
				m_dividerDown = divider;
				break;
			default:
				break;
			}
		}

	protected:
		GUINodeDivider* m_dividerLeft		 = nullptr;
		GUINodeDivider* m_dividerRight		 = nullptr;
		GUINodeDivider* m_dividerUp			 = nullptr;
		GUINodeDivider* m_dividerDown		 = nullptr;
		Rect			m_splitRect			 = Rect();
		Rect			m_totalAvailableRect = Rect();

		StringID m_loadedLeftSID  = 0;
		StringID m_loadedRightSID = 0;
		StringID m_loadedUpSID	  = 0;
		StringID m_loadedDownSID  = 0;
	};

} // namespace Lina::Editor

#endif
