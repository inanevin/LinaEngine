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

#include "GUI/Nodes/Custom/GUINodeCustomLogo.hpp"
#include "GUI/Nodes/Widgets/GUINodeTooltip.hpp"
#include "Data/CommonData.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "Core/Editor.hpp"
#include "Core/Theme.hpp"
#include "System/ISystem.hpp"
#include "Core/SystemInfo.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Graphics/Interfaces/IWindow.hpp"

namespace Lina::Editor
{
	GUINodeCustomLogo::GUINodeCustomLogo(GUIDrawerBase* drawer, int drawOrder) : GUINode(drawer, drawOrder)
	{
		m_sheetItems = m_editor->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager)->GetResource<Texture>("Resources/Editor/Textures/TitleTextAnimation.png"_hs)->GetSheetItems(2, 14);
		m_tooltip	 = new GUINodeTooltip(drawer);

		String tooltipText = "Lina Engine v" + TO_STRING(LINA_MAJOR) + "." + TO_STRING(LINA_MINOR) + "." + TO_STRING(LINA_PATCH) + " Build: " + TO_STRING(LINA_BUILD);
		m_tooltip->SetTitle(tooltipText.c_str());
		AddChildren(m_tooltip);
	}

	void GUINodeCustomLogo::Draw(int threadID)
	{
		if (!GetIsVisible())
			return;

		GUINode::Draw(threadID);

		// Tooltip
		{
			m_tooltip->SetPos(m_window->GetMousePosition() + Vector2i(10, 10));
			m_tooltip->SetVisible(m_isHovered);
		}

		// Setup
		const float	   padding			= Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		const float	   stretchedPadding = padding * 8.0f;
		const Vector2i swapchainSize	= m_swapchain->GetSize();
		const Vector2i itemSize			= m_sheetItems[0].size * 0.35f * m_window->GetDPIScale();
		Vector2		   center			= Vector2(swapchainSize.x * 0.5f, itemSize.y * 0.5f + padding * 0.5f);
		center							= center.Max(m_minRect.pos + Vector2(itemSize.x * 0.5f + padding * 8, 0.0f));
		m_rect.pos						= Vector2(center.x - itemSize.x * 0.5f - stretchedPadding, center.y - itemSize.y * 0.5f);
		m_rect.size						= itemSize + Vector2i(static_cast<int>(stretchedPadding * 2.0f), 0);

		// Background convex shape
		{
			LinaVG::StyleOptions pointsStyle;
			pointsStyle.color		 = LV4(Theme::TC_Dark0);
			pointsStyle.aaMultiplier = 1.0f;

			Vector<LinaVG::Vec2> points;
			points.push_back(LinaVG::Vec2(center.x - itemSize.x * 0.5f - stretchedPadding, 0.0f));
			points.push_back(LinaVG::Vec2(center.x + itemSize.x * 0.5f + stretchedPadding, 0.0f));
			points.push_back(LinaVG::Vec2(center.x + itemSize.x * 0.5f + stretchedPadding - padding * 0.5f, padding + itemSize.y));
			points.push_back(LinaVG::Vec2(center.x - itemSize.x * 0.5f - stretchedPadding + padding * 0.5f, padding + itemSize.y));
			LinaVG::DrawConvex(threadID, points.data(), static_cast<int>(points.size()), pointsStyle, 0.0f, m_drawOrder);
		}

		// Image
		{
			auto draw = [&](TextureSheetItem& item) {
				LinaVG::DrawImage(
					threadID, item.texture->GetSID(), LV2(center), LV2(itemSize), LinaVG::Vec4(1, 1, 1, 1), 0.0f, m_drawOrder, LinaVG::Vec2(1, 1), LinaVG::Vec2(0, 0), LinaVG::Vec2(item.uvTL.x, item.uvTL.y), LinaVG::Vec2(item.uvBR.x, item.uvBR.y));
			};

			if (!m_isHovered)
				draw(m_sheetItems[0]);
			else
			{
				static float counter = 0.0f;
				static int	 index	 = 0;

				if (counter > 0.05f)
				{
					counter = 0.0f;
					index	= (index + 1) % static_cast<int>(m_sheetItems.size());
				}

				draw(m_sheetItems[index]);
				counter += SystemInfo::GetDeltaTimeF();
			}
		}
	}
} // namespace Lina::Editor
