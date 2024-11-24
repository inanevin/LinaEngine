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

#include "Editor/Widgets/Graph/LineGraph.hpp"
#include "Editor/Editor.hpp"
#include "Core/Application.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"

namespace Lina::Editor
{
	void LineGraph::Construct()
	{
		GetWidgetProps().rounding			 = 0.0f;
		GetWidgetProps().outlineThickness	 = Theme::GetDef().baseOutlineThickness;
		GetWidgetProps().childMargins.left	 = Theme::GetDef().baseIndent;
		GetWidgetProps().childMargins.right	 = Theme::GetDef().baseIndent;
		GetWidgetProps().childMargins.bottom = Theme::GetDef().baseIndent;
		GetWidgetProps().childMargins.top	 = Theme::GetDef().baseIndent;
	}

	void LineGraph::Draw()
	{
		return;
		if (m_font == nullptr)
			m_font = Editor::Get()->GetApp()->GetResourceManager().GetResource<Font>(m_props.font);

		LinaVG::TextOptions crossAxisTextOptions;
		crossAxisTextOptions.font				= m_font->GetFont(m_lgxWindow->GetDPIScale());
		crossAxisTextOptions.alignment			= LinaVG::TextAlignment::Right;
		crossAxisTextOptions.color.gradientType = LinaVG::GradientType::Vertical;

		LinaVG::TextOptions mainAxisTextOptions;
		mainAxisTextOptions.font			   = m_font->GetFont(m_lgxWindow->GetDPIScale());
		mainAxisTextOptions.alignment		   = LinaVG::TextAlignment::Left;
		mainAxisTextOptions.color.gradientType = LinaVG::GradientType::Horizontal;

		// Find main axis min & max
		Vector2 mainAxisMaxSize = Vector2::Zero;
		float	mainAxisMin		= 9999.0f;
		float	mainAxisMax		= -9999.0f;
		for (size_t i = 0; i < m_props.mainAxisPoints.size(); i++)
		{
			AxisPoint& point = m_props.mainAxisPoints.at(i);

			if (!point.displayName.empty())
			{
				point._calculatedSize = m_lvg->CalculateTextSize(point.displayName.c_str(), mainAxisTextOptions);
				mainAxisMaxSize		  = mainAxisMaxSize.Max(point._calculatedSize);
			}

			if (point.value < mainAxisMin)
				mainAxisMin = point.value;

			if (point.value > mainAxisMax)
				mainAxisMax = point.value;
		}

		// Find cross axis min & max
		Vector2 crossAxisMaxSize = Vector2::Zero;
		float	crossAxisMin	 = 9999.0f;
		float	crossAxisMax	 = -9999.0f;
		if (m_props.crossAxisDynamic)
		{
			for (const PointGroup& group : m_props.groups)
			{
				for (const PointData& point : group.points)
				{
					if (point.crossAxisValue < crossAxisMin)
						crossAxisMin = point.crossAxisValue;

					if (point.crossAxisValue > crossAxisMax)
						crossAxisMax = point.crossAxisValue;

					const String str = UtilStr::FloatToString(point.crossAxisValue, m_props.crossAxisDynamicDecimals);
					crossAxisMaxSize = crossAxisMaxSize.Max(m_lvg->CalculateTextSize(str.c_str(), crossAxisTextOptions));
				}
			}

			crossAxisMax += crossAxisMax * m_props.crossAxisDynamicScale;
			crossAxisMin -= crossAxisMin * m_props.crossAxisDynamicScale;
			if (crossAxisMin < m_props.crossAxisDynamicClampMin)
				crossAxisMin = m_props.crossAxisDynamicClampMin;
		}
		else
		{
			for (size_t i = 0; i < m_props.crossAxisPoints.size(); i++)
			{
				AxisPoint& point = m_props.crossAxisPoints.at(i);

				if (!point.displayName.empty())
				{
					point._calculatedSize = m_lvg->CalculateTextSize(point.displayName.c_str(), crossAxisTextOptions);
					crossAxisMaxSize	  = crossAxisMaxSize.Max(point._calculatedSize);
				}

				if (point.value < crossAxisMin)
					crossAxisMin = point.value;

				if (point.value > crossAxisMax)
					crossAxisMax = point.value;
			}
		}

		const float afterTextIndent = Theme::GetDef().baseIndentInner;

		LinaVG::StyleOptions opts;

		const Vector2 start		  = GetStartFromMargins();
		const Vector2 end		  = GetEndFromMargins();
		const Vector2 topLeft	  = Vector2(start.x, start.y + mainAxisMaxSize.y + afterTextIndent) + Vector2(crossAxisMaxSize.x + afterTextIndent, 0.0f);
		const Vector2 bottomLeft  = Vector2(topLeft.x, end.y) - Vector2(0.0f, mainAxisMaxSize.y + afterTextIndent);
		const Vector2 bottomRight = Vector2(end.x - crossAxisMaxSize.x - afterTextIndent, bottomLeft.y);
		const Vector2 topRight	  = Vector2(bottomRight.x, topLeft.y);
		const Vector2 graphSize	  = bottomRight - topLeft;

		const float colorInternalLerpFactor = 0.2f;

		// Title
		if (!m_props.title.empty())
		{
			LinaVG::TextOptions opts;
			opts.font		  = m_font->GetFont(m_lgxWindow->GetDPIScale());
			opts.alignment	  = LinaVG::TextAlignment::Center;
			const Vector2 pos = Vector2(topLeft.x + graphSize.x * 0.5f, topLeft.y - afterTextIndent);
			m_lvg->DrawTextDefault(m_props.title.c_str(), pos.AsLVG(), opts, 0.0f, m_drawOrder, false);
		}

		// Cross axis text.
		if (m_props.crossAxisDynamic)
		{
			const Vector<float> points = {crossAxisMax, (crossAxisMax + crossAxisMin) * 0.5f, crossAxisMin};
			for (float point : points)
			{
				const float	  ratio	   = Math::Remap(point, crossAxisMin, crossAxisMax, 1.0f, 0.0f);
				const String  str	   = UtilStr::FloatToString(point, m_props.crossAxisDynamicDecimals);
				const Vector2 size	   = m_lvg->CalculateTextSize(str.c_str(), crossAxisTextOptions);
				const Vector2 position = Vector2(topLeft.x - afterTextIndent, topLeft.y + graphSize.y * ratio + size.y * .5f);

				if (m_props.colorTextCrossAxisInterpolate)
				{
					const Color baseColor			 = Math::Lerp(m_props.colorTextCrossAxis.start, m_props.colorTextCrossAxis.end, ratio);
					crossAxisTextOptions.color.start = Math::Lerp(baseColor, m_props.colorTextCrossAxis.start, colorInternalLerpFactor).AsLVG4();
					crossAxisTextOptions.color.end	 = Math::Lerp(baseColor, m_props.colorTextCrossAxis.end, colorInternalLerpFactor).AsLVG4();
				}
				else
					crossAxisTextOptions.color = m_props.colorTextCrossAxis.AsLVG();

				m_lvg->DrawTextDefault(str.c_str(), position.AsLVG(), crossAxisTextOptions, 0.0f, m_drawOrder, true);
			}
		}
		else
		{
			for (const AxisPoint& point : m_props.crossAxisPoints)
			{
				if (point.displayName.empty())
					continue;

				const float	  ratio	   = Math::Remap(point.value, crossAxisMin, crossAxisMax, 1.0f, 0.0f);
				const Vector2 position = Vector2(topLeft.x - afterTextIndent, topLeft.y + graphSize.y * ratio + point._calculatedSize.y * .5f);

				if (m_props.colorTextCrossAxisInterpolate)
				{
					const Color baseColor			 = Math::Lerp(m_props.colorTextCrossAxis.start, m_props.colorTextCrossAxis.end, ratio);
					crossAxisTextOptions.color.start = Math::Lerp(baseColor, m_props.colorTextCrossAxis.start, colorInternalLerpFactor).AsLVG4();
					crossAxisTextOptions.color.end	 = Math::Lerp(baseColor, m_props.colorTextCrossAxis.end, colorInternalLerpFactor).AsLVG4();
				}
				else
					crossAxisTextOptions.color = m_props.colorTextCrossAxis.AsLVG();

				m_lvg->DrawTextDefault(point.displayName.c_str(), position.AsLVG(), crossAxisTextOptions, 0.0f, m_drawOrder, true);
			}
		}

		// Main axis text
		for (const AxisPoint& point : m_props.mainAxisPoints)
		{
			if (point.displayName.empty())
				continue;

			const float	  ratio	   = Math::Remap(point.value, mainAxisMin, mainAxisMax, 0.0f, 1.0f);
			const Vector2 position = Vector2(bottomLeft.x + graphSize.x * ratio - point._calculatedSize.x * 0.5f, bottomLeft.y + afterTextIndent + mainAxisMaxSize.y);

			if (m_props.colorTextMainAxisInterpolate)
			{
				const Color baseColor			= Math::Lerp(m_props.colorTextMainAxis.start, m_props.colorTextMainAxis.end, ratio);
				mainAxisTextOptions.color.start = Math::Lerp(baseColor, m_props.colorTextMainAxis.start, colorInternalLerpFactor).AsLVG4();
				mainAxisTextOptions.color.end	= Math::Lerp(baseColor, m_props.colorTextMainAxis.end, colorInternalLerpFactor).AsLVG4();
			}
			else
				mainAxisTextOptions.color = m_props.colorTextMainAxis.AsLVG();

			m_lvg->DrawTextDefault(point.displayName.c_str(), position.AsLVG(), mainAxisTextOptions, 0.0f, m_drawOrder, true);
		}

		// Grid background.
		if (m_props.drawBackground)
		{
			LinaVG::StyleOptions bgOpts;
			bgOpts.color = m_props.colorBackground.AsLVG();
			m_lvg->DrawRect(topLeft.AsLVG(), bottomRight.AsLVG(), bgOpts, 0.0f, m_drawOrder);
		}

		// Cross-axis line
		opts.color	   = m_props.colorBarCrossAxis.AsLVG();
		opts.thickness = m_props.crossAxisThickness;
		m_lvg->DrawLine(topLeft.AsLVG(), bottomLeft.AsLVG(), opts, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);

		// Main axis baseline.
		opts.color	   = m_props.colorBarMainAxis.AsLVG();
		opts.thickness = m_props.mainAxisThickness;
		m_lvg->DrawLine(bottomLeft.AsLVG(), bottomRight.AsLVG(), opts, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);

		// Vertical grid lines
		if (m_props.gridCells.x != 0)
		{
			const float			 gridXDivision = graphSize.x / static_cast<float>(m_props.gridCells.x);
			LinaVG::StyleOptions lineStyle;
			lineStyle.color		= m_props.colorGridVertical.AsLVG();
			lineStyle.thickness = m_props.gridThickness;
			for (uint32 i = 0; i < m_props.gridCells.x; i++)
			{
				const float startX = bottomLeft.x + gridXDivision * (i + 1);
				m_lvg->DrawLine(LinaVG::Vec2(startX, topLeft.y), LinaVG::Vec2(startX, bottomLeft.y), lineStyle, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);
			}
		}

		// Horizontal grid lines
		if (m_props.gridCells.y != 0)
		{
			const float			 gridYDivision = graphSize.y / static_cast<float>(m_props.gridCells.y);
			LinaVG::StyleOptions lineStyle;
			lineStyle.color		= m_props.colorGridHorizontal.AsLVG();
			lineStyle.thickness = m_props.gridThickness;

			for (uint32 i = 0; i < m_props.gridCells.y; i++)
			{
				const float startY = bottomLeft.y - gridYDivision * (i + 1);
				m_lvg->DrawLine(LinaVG::Vec2(topLeft.x, startY), LinaVG::Vec2(bottomRight.x, startY), lineStyle, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);
			}
		}

		LinaVG::StyleOptions pointStyle;

		for (const PointGroup& group : m_props.groups)
		{
			pointStyle.thickness = group.thickness;
			pointStyle.aaEnabled = true;

			Vector<PointData> sortedPoints = group.points;
			linatl::sort(sortedPoints.begin(), sortedPoints.end(), [](const PointData& p1, const PointData& p2) -> bool { return p1.mainAxisValue < p2.mainAxisValue; });

			Vector2 lastPoint = Vector2::Zero;

			m_groupPoints.resize(0);

			for (size_t i = 0; i < sortedPoints.size(); i++)
			{
				const PointData& point	= sortedPoints[i];
				const float		 yRatio = Math::Remap(point.crossAxisValue, crossAxisMin, crossAxisMax, 1.0f, 0.0f);
				const float		 xRatio = Math::Remap(point.mainAxisValue, mainAxisMin, mainAxisMax, 0.0f, 1.0f);

				const LinaVG::Vec2 pointPos = LinaVG::Vec2(bottomLeft.x + graphSize.x * xRatio, topLeft.y + graphSize.y * yRatio);
				// if (i == 0)
				// 	m_groupPoints.push_back(LinaVG::Vec2(bottomLeft.x, pointPos.y));
				m_groupPoints.push_back(pointPos);
			}

			if (group.drawLine && m_groupPoints.size() > 1)
			{
				pointStyle.color = group.colorLine.AsLVG();

				if (m_groupPoints.size() < 3)
					m_lvg->DrawLine(m_groupPoints[0], m_groupPoints[1], pointStyle, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);
				else
					m_lvg->DrawLines(m_groupPoints.data(), m_groupPoints.size(), pointStyle, LinaVG::LineCapDirection::None, LinaVG::LineJointType::Bevel, m_drawOrder);
			}

			if (group.drawPoint)
			{
				pointStyle.color = group.colorPoint.AsLVG();
				for (size_t i = 1; i < m_groupPoints.size(); i++)
					m_lvg->DrawCircle(m_groupPoints[i], 5.0f, pointStyle, 18, 0.0f, 0.0f, 360.0f, m_drawOrder + 1);
			}
		}

		LinaVG::TextOptions legendTextOpts;
		legendTextOpts.font		 = m_font->GetFont(m_lgxWindow->GetDPIScale());
		legendTextOpts.alignment = LinaVG::TextAlignment::Right;

		LinaVG::StyleOptions legendRectOpts;

		const float legendRectSize = legendTextOpts.font->size;

		Vector2 legendPos = topRight + Vector2(-afterTextIndent, afterTextIndent);

		for (Legend& legend : m_props.legends)
		{
			legendTextOpts.color = m_props.colorLegendText.AsLVG4();

			const Vector2 sz = m_lvg->CalculateTextSize(legend.text.c_str(), legendTextOpts);
			m_lvg->DrawTextDefault(legend.text.c_str(), Vector2(legendPos.x, legendPos.y + sz.y).AsLVG(), legendTextOpts, 0.0f, m_drawOrder);

			if (legend.axis.empty())
			{
				legendRectOpts.color = legend.color.AsLVG4();

				const Vector2 tl = Vector2(legendPos.x - sz.x - afterTextIndent - legendRectSize, legendPos.y);
				const Vector2 br = Vector2(tl.x + legendRectSize, tl.y + legendRectSize);
				m_lvg->DrawRect(tl.AsLVG(), br.AsLVG(), legendRectOpts, 0.0f, m_drawOrder);
			}
			else
			{
				legendTextOpts.color = legend.color.AsLVG4();
				m_lvg->DrawTextDefault(legend.axis.c_str(), Vector2(legendPos.x - sz.x - afterTextIndent, legendPos.y + sz.y).AsLVG(), legendTextOpts, 0.0f, m_drawOrder);
			}

			legendPos.y += legendTextOpts.font->size + afterTextIndent;
		}
	}
} // namespace Lina::Editor
