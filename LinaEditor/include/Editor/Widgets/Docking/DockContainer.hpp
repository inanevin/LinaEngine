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

#include "Core/GUI/Widgets/Widget.hpp"
#include "Editor/CommonEditor.hpp"
#include "Common/Common.hpp"

namespace Lina::Editor
{
	class DockArea;
	class DockPreview;
	class DockBorder;

	class DockContainer : public Widget
	{
	public:
		DockContainer()			 = default;
		virtual ~DockContainer() = default;

		static constexpr float DEFAULT_DOCK_PERC = 0.15f;

		struct Properties
		{
			Color colorBackground = Theme::GetDef().background0;
		};

		virtual void Initialize() override;
		virtual void Tick(float delta) override;
		virtual void Draw(int32 threadIndex) override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction action) override;
		DockArea*	 AddDockArea(Direction direction);
		void		 ShowPreview();
		void		 HidePreview();
		void		 AreaSortHorizontal(Vector<DockArea*>& outAreas);
		void		 AreaSortVertical(Vector<DockArea*>& outAreas);

		inline Properties& GetProps()
		{
			return m_props;
		}

		inline const Vector<DockArea*>& GetDockAreas() const
		{
			return m_dockAreas;
		}

		inline const Vector<DockBorder*>& GetSortedVerticalBorders() const
		{
			return m_verticalBorders;
		}

		inline const Vector<DockBorder*>& GetSortedHorizontalBorders() const
		{
			return m_horizontalBorders;
		}

	private:
		DockBorder* CreateDockBorder(const Rect& alignRect, DirectionOrientation orientation);
		void		FindAreasPosAlign(Vector<DockArea*>& outAreas, float align, DirectionOrientation direction, bool lookForEnd);

	private:
		Properties			m_props	  = {};
		DockPreview*		m_preview = nullptr;
		Vector<DockArea*>	m_dockAreas;
		Vector<DockBorder*> m_horizontalBorders;
		Vector<DockBorder*> m_verticalBorders;
	};

} // namespace Lina::Editor
