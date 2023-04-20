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

#ifndef GUINodeDivider_HPP
#define GUINodeDivider_HPP

#include "GUI/Nodes/GUINode.hpp"
#include "Core/EditorCommon.hpp"
#include "Data/Vector.hpp"

namespace Lina::Editor
{
	class GUINodeDivisible;

	class GUINodeDivider : public GUINode
	{
	public:
		GUINodeDivider(GUIDrawerBase* drawer, int drawOrder) : GUINode(drawer, drawOrder){};
		virtual ~GUINodeDivider() = default;

		virtual void Draw(int threadID) override;
		virtual void OnHoverBegin() override;
		virtual void OnHoverEnd() override;
		virtual void OnDragEnd() override;
		void		 PreDestroy(GUINodeDivisible* divisible);
		void		 AddPositiveNode(GUINodeDivisible* node);
		void		 AddNegativeNode(GUINodeDivisible* node);
		void		 RemovePositiveNode(GUINodeDivisible* node);
		void		 RemoveNegativeNode(GUINodeDivisible* node);
		bool		 IsOnPositiveSide(GUINodeDivisible* div);

		inline void SetDragDirection(Direction dir)
		{
			m_dragDirection = dir;
		}

		inline const Vector<GUINodeDivisible*>& GetPositiveNodes() const
		{
			return m_positiveNodes;
		}

		inline const Vector<GUINodeDivisible*>& GetNegativeNodes() const
		{
			return m_negativeNodes;
		}

	private:
		Direction				  m_dragDirection = Direction::Horizontal;
		Vector<GUINodeDivisible*> m_positiveNodes;
		Vector<GUINodeDivisible*> m_negativeNodes;
	};

} // namespace Lina::Editor

#endif
