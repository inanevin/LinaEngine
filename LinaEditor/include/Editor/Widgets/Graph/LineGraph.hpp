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
#include "Core/Resources/CommonResources.hpp"

namespace Lina
{
	class Font;
};

namespace Lina::Editor
{

	class LineGraph : public Widget
	{
	public:
		enum class GraphStyle
		{
			Line,
			Point,
		};

		struct AxisPoint
		{
			float  value	   = 0.0f;
			String displayName = "";

			Vector2 _calculatedSize = Vector2::Zero;
		};

		struct PointData
		{
			float crossAxisValue = 0.0f;
			float mainAxisValue	 = 0.0f;
		};

		struct PointGroup
		{
			ColorGrad		  colorLine	 = Theme::GetDef().accentPrimary0;
			ColorGrad		  colorPoint = Theme::GetDef().accentPrimary0;
			float			  thickness	 = Theme::GetDef().baseOutlineThickness;
			bool			  drawLine	 = true;
			bool			  drawPoint	 = false;
			Vector<PointData> points	 = {};
		};

		struct Legend
		{
			String	  text	= "";
			ColorGrad color = Theme::GetDef().accentPrimary0;

			Vector2 _calculatedSize = Vector2::Zero;
		};

		LineGraph()			 = default;
		virtual ~LineGraph() = default;

		struct Properties
		{
			Color colorLegendText = Theme::GetDef().foreground1;

			Vector<AxisPoint> mainAxisPoints;
			Vector<AxisPoint> crossAxisPoints;
			Vector<Legend>	  legends;

			float crossAxisThickness = Theme::GetDef().baseOutlineThickness;
			float mainAxisThickness	 = Theme::GetDef().baseOutlineThickness;
			float gridThickness		 = Theme::GetDef().baseOutlineThickness;

			bool	  colorTextCrossAxisInterpolate = false;
			bool	  colorTextMainAxisInterpolate	= false;
			ColorGrad colorTextCrossAxis			= ColorGrad(Theme::GetDef().foreground0);
			ColorGrad colorTextMainAxis				= ColorGrad(Theme::GetDef().foreground0);

			ColorGrad colorBarCrossAxis = Theme::GetDef().silent0;
			ColorGrad colorBarMainAxis	= Theme::GetDef().silent0;

			bool	  drawGridBackground = false;
			ColorGrad colorBackground	 = Theme::GetDef().background1;

			Vector2ui gridCells			  = Vector2ui::Zero;
			ColorGrad colorGridVertical	  = Theme::GetDef().silent1;
			ColorGrad colorGridHorizontal = Theme::GetDef().silent1;

			Vector<PointGroup> groups;
			ResourceID		   font = Theme::GetDef().defaultFont;

			bool   crossAxisDynamic			= false;
			float  crossAxisDynamicScale	= 1.25f;
			float  crossAxisDynamicClampMin = 0.0f;
			uint32 crossAxisDynamicDecimals = 0;

			String title	  = "";
			Color  colorTitle = Theme::GetDef().foreground0;
		};

		virtual void Draw() override;

		inline Properties& GetProps()
		{
			return m_props;
		}

	private:
		Vector<LinaVG::Vec2> m_groupPoints;
		Properties			 m_props = {};
		Font*				 m_font	 = nullptr;
	};

	LINA_WIDGET_BEGIN(LineGraph, Graph)
	LINA_CLASS_END(LineGraph)
} // namespace Lina::Editor
