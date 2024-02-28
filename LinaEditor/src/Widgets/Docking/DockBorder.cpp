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

#include "Editor/Widgets/Docking/DockBorder.hpp"
#include "Editor/Widgets/Docking/DockContainer.hpp"
#include "Editor/Widgets/Docking/DockArea.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina::Editor
{
	void DockBorder::Construct()
	{
	}

	void DockBorder::Tick(float delta)
	{
	}

	void DockBorder::Draw(int32 threadIndex)
	{
	}

	void DockBorder::DebugDraw(int32 threadIndex, int32 drawOrder)
	{
		LinaVG::StyleOptions opts;
		opts.color	   = Color::Red.AsLVG4();
		opts.isFilled  = false;
		opts.thickness = 2.0f;

		const Vector2 absPos  = m_alignRect.pos * m_parentContainer->GetSize();
		const Vector2 absSize = m_alignRect.size * m_parentContainer->GetSize();
		const Vector2 end	  = absPos + absSize;
		LinaVG::DrawLine(threadIndex, absPos.AsLVG(), end.AsLVG(), opts, LinaVG::LineCapDirection::None, 0.0f, drawOrder + 1);
	}

	bool DockBorder::CanResizeArea(DockArea* area, float amt)
	{
		return false;
		// // Push right pull left
		// if(m_orientation == DirectionOrientation::Vertical)
		//     return area->m_alignRect.size.x - amt > DockArea::MIN_SIZE_PERC;
		// else
		//     return area->m_alignRect.size.y - amt > DockArea::MIN_SIZE_PERC;
	}

	void DockBorder::GetClippingAreas(Vector<DockArea*>& outAreas, bool isPush)
	{
		// outAreas.clear();
		// const Vector2 absPos = m_alignRect.pos * m_parentContainer->GetSize();
		// const Vector2 absSize = m_alignRect.size * m_parentContainer->GetSize();
		// const Vector2 pushDirection = Vector2(1.0f - m_alignRect.size.x, 1.0f - m_alignRect.size.y).Normalized() * (isPush ? 2.0f : -2.0f);
		// const Vector2 testSize = m_alignRect.size.Normalized() * absSize + Vector2(1.0f - m_alignRect.size.x, 1.0f - m_alignRect.size.y).Normalized();
		// const Vector2 testPos = absPos + pushDirection;
		// const Rect testRect = Rect(testPos, testSize);
		// const Vector<DockArea*>& areas = m_parentContainer->GetDockAreas();
		//
		// for(auto* area : areas)
		// {
		//     if(area->GetRect().IsClipping(testRect))
		//         outAreas.push_back(area);
		// }
	}

	void DockBorder::PullBy(float amt)
	{
	}

	void DockBorder::PushBy(float amt)
	{
		// Vector<DockArea*> clippingAreas;
		// GetClippingAreas(clippingAreas, true);
		//
		// if(m_orientation == DirectionOrientation::Vertical)
		// {
		//     const Vector<DockBorder*>& verticalBorders = m_parentContainer->GetSortedVerticalBorders();
		//     auto me = linatl::find(verticalBorders.begin(), verticalBorders.end(), this);

		//     for(auto* area : clippingAreas)
		//     {
		//         area->m_alignRect.pos.x += amt;
		//         area->m_alignRect.size.x -= amt;
		//     }
		//
		//     // Push meself
		//     m_alignRect.pos.x += amt;
		// }
		// else
		// {
		//     const Vector<DockBorder*>& horizontalBorders = m_parentContainer->GetSortedVerticalBorders();
		//     auto me = linatl::find(horizontalBorders.begin(), horizontalBorders.end(), this);

		//     for(auto* area : clippingAreas)
		//     {
		//         area->m_alignRect.pos.y += amt;

		//         if(CanResizeArea(area, amt))
		//             area->m_alignRect.size.y -= amt;
		//         else
		//             area->FindBorder(Direction::Bottom)->PushBy(amt);
		//     }
		//
		//     // Push meself
		//     m_alignRect.pos.y += amt;
		// }
	}

	void PullBy(float amt)
	{
	}

} // namespace Lina::Editor
