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

#include "Core/Lina.hpp"
#include "FlarePackerAppDelegate.hpp"
#include "Common/GUI/Widgets/Layout/FreeRoam.hpp"
#include "Common/GUI/Widgets/Layout/Column.hpp"
#include "Common/GUI/Widgets/Layout/Row.hpp"
#include "Common/GUI/WidgetAllocator.hpp"
#include "Common/GUI/Widgets/Container/Box.hpp"
#include "Common/GUI/Widgets/Container/Background.hpp"
#include "Common/GUI/Widgets/Text/Text.hpp"
#include "Common/GUI/Widgets/Text/Icon.hpp"
#include "Common/GUI/Widgets/Composite/WindowButtons.hpp"

namespace Lina
{

	SystemInitializationInfo Lina_GetInitInfo()
	{
		return SystemInitializationInfo{
			.appName			 = "Flare Packer",
			.windowWidth		 = 800,
			.windowHeight		 = 600,
			.windowStyle		 = LinaGX::WindowStyle::WindowedApplication,
			.appListener		 = new Lina::FlarePackerAppDelegate(),
			.resourceManagerMode = Lina::ResourceManagerMode::File,
		};
	}

	void FlarePackerAppDelegate::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
	}

	void FillTitleContents(FreeRoam* titleBar)
	{
		// Row* row = titleBar->AllocateChild<Row>();
		// row->SetCrossAlignment(CrossAlignment::Center);
		//
		// Background* rowBg = titleBar->AllocateFree<Background>();
		// row->SetBackground(rowBg);

		/*
		 WindowButtons* wb = ...
		 windowButtons

		 */

		/*

		 row->contents = {
		 .crossAlignment = CrossALignment::Center,
		 .background = BackgroundStyle(),
		 .children = { title, centerPiece, windowButtons },
		 }

		 */
	}

	Widget* GetTitleRow(Widget* titleBar)
	{
		Row* row = titleBar->Allocate<Row>();

		// Title text.
		Text* title		= row->Allocate<Text>();
		title->contents = {
			.text = "Flare Packer",
			.font = nullptr,
		};

		// Title icon
		Text* icon	   = row->Allocate<Text>();
		icon->contents = {
			.text = "IC0",
			.font = nullptr,
		};

		// Sub-title row
		Row* titleRow	   = row->Allocate<Row>();
		titleRow->contents = {
			.mainAlignment	= MainAlignment::Free,
			.crossAlignment = CrossAlignment::Center,
		};
		titleRow->children = {title, icon};

		// Window buttons
		WindowButtons* wb = row->Allocate<WindowButtons>();
		wb->contents	  = {
				 .onClickedMinimize = []() {},
				 .onClickedMaximize = []() {},
				 .onClickedExit		= []() {},
		 };

		// Main row
		row->contents = {
			.mainAlignment	= MainAlignment::SpaceBetween,
			.crossAlignment = CrossAlignment::Center,
		};

		row->children = {titleRow, wb};

		return row;
	}

	void FlarePackerAppDelegate::RenderSurfaceOverlay(LinaGX::CommandStream* cmdStream, LinaGX::Window* window, int32 threadIndex)
	{
		const float monitorHeight	= static_cast<float>(window->GetMonitorSize().y);
		const float titleBarHeight	= monitorHeight * 0.02f;
		const float bottomBarHeight = monitorHeight * 0.2f;

		FreeRoam* titleBar = WidgetAllocator::Get().Allocate<FreeRoam>(threadIndex, window);

		titleBar->transformation = {
			.pos  = Vector2::Zero,
			.size = Vector2(window->GetSize().x, titleBarHeight),
		};

		titleBar->children = {GetTitleRow(titleBar)};

		titleBar->SizePass();
		titleBar->Draw();

		/*

		 titleBar->transformation = {
			.size = size,
			.pos = pos,
		 };

		 titleBar->contents = {
			.child = GetTitleBarContents(titleBar),
		 };

		 */

		// FreeRoam* mainSection = WidgetAllocator::Get().Allocate<FreeRoam>(threadIndex, window);
		// mainSection->SetPosition(0, titleBarHeight);
		// mainSection->SetSize(window->GetSize().x, window->GetSize().y - titleBarHeight - bottomBarHeight);
		//
		// FreeRoam* bottomSection = WidgetAllocator::Get().Allocate<FreeRoam>(threadIndex, window);
		// bottomSection->SetPosition(0, window->GetSize().y - bottomBarHeight);
		// bottomSection->SetSize(window->GetSize().x, bottomBarHeight);

		// // Column
		// {
		//     Column* col = root->AllocateChild<Column>();
		//     col->SetPadding(125);
		//     col->SetMargins(MARGINS_EQ(10));
		//     col->SetMainAlignment(MainAlignment::Free);
		//     col->SetCrossAlignment(CrossAlignment::Start);

		//     // Children
		//     {
		//         Column* colN = col->AllocateChild<Column>();
		//         colN->SetMainAlignment(MainAlignment::Free);
		//         colN->SetCrossAlignment(CrossAlignment::Start);
		//         colN->SetMargins(MARGINS_EQ(4));
		//
		//         {
		//             Box* b1 = colN->AllocateChild<Box>();
		//             b1->SetSize(Vector2(20, 20));
		//         }
		//
		//         Column* colN2 = col->AllocateChild<Column>();
		//         colN2->SetMainAlignment(MainAlignment::Free);
		//         colN2->SetCrossAlignment(CrossAlignment::Start);
		//         colN2->SetMargins(MARGINS_EQ(4));

		//         {
		//             Box* b1 = colN2->AllocateChild<Box>();
		//             b1->SetSize(Vector2(20, 20));
		//         }
		//     }
		// }

		// titleBar->CalculateDesiredSize();
		// mainSection->CalculateDesiredSize();
		// bottomSection->CalculateDesiredSize();

		// titleBar->Draw();
		// mainSection->Draw();
		// bottomSection->Draw();
	}

} // namespace Lina
