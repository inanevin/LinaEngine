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

#include "Editor/Widgets/Panel/PanelWorld.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/System/System.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Common/Math/Math.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/World/WorldManager.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"

#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
#define CIRCLE_RAD		 90
#define CIRCLE_THICKNESS 8
#define WARNING_RAD		 90

#define TITLE_CIRCLE_OFFSET	 Vector2(140, 140)
#define WARNING_OFFSET		 Vector2(-280, 140)
#define TITLE_TEXT_OFFSET	 Vector2(280, 140)
#define PAGE_CONTENTS_OFFSET Vector2(280, 280)
#define TIMELINE_OFFSET		 Vector2(140, 280)
#define TIMELINE_THICKNESS	 5.0f

#define CONTENT_SIZE_PERC Vector2(0.6f, 0.6f)

	int				demoDrawOrder = 0;
	LinaVG::Drawer* demoDrawer;

	Vector2				currentPagePosition = Vector2::Zero;
	Vector2				originOffset		= Vector2::Zero;
	Vector2				widgetSize			= Vector2::Zero;
	LinaVG::LinaVGFont* titleFont			= nullptr;
	LinaVG::LinaVGFont* titleAltFont		= nullptr;
	int32				currentPage			= 0;
	float				originOffsetTargetX = 0.0f;

	Texture* textureLinaECS		   = nullptr;
	Texture* textureLinaPBR		   = nullptr;
	Texture* textureLinaPBR2	   = nullptr;
	Texture* textureLinaPBR3	   = nullptr;
	Texture* textureLinaDemoEditor = nullptr;
	Texture* textureLina7DFPS	   = nullptr;
	Texture* textureLinaNewEditor  = nullptr;
	Texture* textureLinaVG		   = nullptr;
	Texture* textureLinaGX		   = nullptr;
	Texture* textureDX12		   = nullptr;

	Vector2 GetCurrentPos()
	{
		return originOffset + currentPagePosition;
	}

	float BulletPointContent(const Vector<String>& items, float xOffset = 0.0f)
	{
		Vector2 start = GetCurrentPos() + PAGE_CONTENTS_OFFSET + Vector2(xOffset, 0.0f);

		LinaVG::TextOptions opts;
		opts.color = Theme::GetDef().foreground0.AsLVG4();
		opts.font  = titleAltFont;

		LinaVG::StyleOptions circle;
		circle.aaEnabled = true;

		int	  i	   = 0;
		float maxX = 0.0f;
		for (const auto& str : items)
		{
			const Vector2 sz = demoDrawer->CalculateTextSize(str.c_str(), opts);

			if (sz.x > maxX)
				maxX = sz.x;

			if (i == 0)
				start.y += sz.y;
			demoDrawer->DrawTextNormal(str.c_str(), start.AsLVG(), opts, 0.0f, demoDrawOrder);

			demoDrawer->DrawCircle((start - Vector2(20.0f, sz.y / 2.0f)).AsLVG(), 5.0f, circle, 36, 0.0f, 0.0f, 360.0f, demoDrawOrder);
			start.y += sz.y + 30.0f;
			i++;
		}

		return maxX;
	}

	void DrawTitleCircleAnimated()
	{
		LinaVG::StyleOptions opts;
		opts.isFilled	 = false;
		opts.thickness	 = CIRCLE_THICKNESS;
		opts.aaEnabled	 = true;
		opts.color.start = Theme::GetDef().accentPrimary0.AsLVG4();
		opts.color.end	 = Theme::GetDef().accentPrimary1.AsLVG4();

		static float rotation = 0.0f;
		rotation += 0.016f * 35.0f;

		demoDrawer->DrawCircle((GetCurrentPos() + TITLE_CIRCLE_OFFSET).AsLVG(), CIRCLE_RAD, opts, 36, rotation, 0.0f, 270.0f, demoDrawOrder);

		opts.color.start = Theme::GetDef().accentSecondary.AsLVG4();
		opts.color.end	 = Theme::GetDef().accentWarn.AsLVG4();

		static float rotation2 = 30.0f;
		rotation2 += 0.016f * 155.0f;

		opts.thickness = CIRCLE_THICKNESS / 2;
		demoDrawer->DrawCircle((GetCurrentPos() + TITLE_CIRCLE_OFFSET).AsLVG(), CIRCLE_RAD / 2, opts, 36, rotation2, 0.0f, 320.0f, demoDrawOrder);
	}

	void DrawTitleCircleDate(const String& year, const String& month)
	{
		LinaVG::StyleOptions opts;
		opts.isFilled	 = false;
		opts.thickness	 = CIRCLE_THICKNESS;
		opts.aaEnabled	 = true;
		opts.color.start = Theme::GetDef().accentPrimary0.AsLVG4();
		opts.color.end	 = Theme::GetDef().accentPrimary1.AsLVG4();

		static float rotation = 0.0f;
		rotation += 0.016f * 35.0f;

		const Vector2 circleCenter = GetCurrentPos() + TITLE_CIRCLE_OFFSET;
		demoDrawer->DrawCircle(circleCenter.AsLVG(), CIRCLE_RAD, opts, 36, rotation, 0.0f, 270.0f, demoDrawOrder);

		LinaVG::TextOptions txtOpts;
		txtOpts.color	  = Theme::GetDef().foreground0.AsLVG4();
		txtOpts.alignment = LinaVG::TextAlignment::Center;
		txtOpts.font	  = titleAltFont;

		const Vector2 monthSz  = demoDrawer->CalculateTextSize(month.c_str(), txtOpts);
		const Vector2 yearPos  = circleCenter - Vector2(0.0f, 10.0f);
		const Vector2 monthPos = circleCenter + Vector2(0.0f, 10.0f + monthSz.y);
		demoDrawer->DrawTextNormal(year.c_str(), yearPos.AsLVG(), txtOpts, 0.0f, demoDrawOrder);
		demoDrawer->DrawTextNormal(month.c_str(), monthPos.AsLVG(), txtOpts, 0.0f, demoDrawOrder);
	}

	Vector2 DrawImageContent(Texture* txt, bool drawDropShadow = true, float sizeMultip = 1.0f)
	{
		const Vector2 contentSize = widgetSize * CONTENT_SIZE_PERC * sizeMultip;
		const float	  aspect	  = (txt->GetSizeF().x / txt->GetSizeF().y);

		Vector2 imgSize = Vector2::Zero;
		if (txt->GetSizeF().x > txt->GetSizeF().y)
		{
			imgSize = Vector2(contentSize.x, contentSize.x / aspect);
		}
		else
			imgSize = Vector2(contentSize.y * aspect, contentSize.y);

		const Vector2 start = GetCurrentPos() + PAGE_CONTENTS_OFFSET;
		const Vector2 end	= start + imgSize;

		LinaVG::StyleOptions dd;
		dd.color = Theme::GetDef().background0.AsLVG4();

		if (drawDropShadow)
		{
			demoDrawer->DrawRect((start + Vector2(30, 30)).AsLVG(), (end + Vector2(30, 30)).AsLVG(), dd, 0.0f, demoDrawOrder);
			demoDrawer->DrawImage(txt->GetBindlessIndex() + 1, ((end + start) / 2).AsLVG(), (imgSize).AsLVG(), LinaVG::Vec4(1, 1, 1, 1), 0.0f, demoDrawOrder + 1);
		}
		else
		{
			demoDrawer->DrawImage(txt->GetBindlessIndex() + 1, ((end + start) / 2).AsLVG(), (imgSize).AsLVG(), LinaVG::Vec4(1, 1, 1, 1), 0.0f, demoDrawOrder);
		}
		return imgSize;
	}

	void DrawTimelineLine()
	{
		LinaVG::StyleOptions opts;
		opts.color.start		= Theme::GetDef().accentPrimary0.AsLVG4();
		opts.color.end			= Theme::GetDef().accentPrimary1.AsLVG4();
		opts.color.gradientType = LinaVG::GradientType::Vertical;

		const Vector2 start = GetCurrentPos() + TIMELINE_OFFSET - Vector2(TIMELINE_THICKNESS, 0.0f);
		const Vector2 end	= GetCurrentPos() + TIMELINE_OFFSET + Vector2(TIMELINE_THICKNESS, widgetSize.y - TIMELINE_OFFSET.y);

		demoDrawer->DrawRect(start.AsLVG(), end.AsLVG(), opts, 0.0f, demoDrawOrder + 1);
	}

	void DrawTitle(const String& title, const String& altTitle)
	{
		const Vector2 pos = GetCurrentPos() + TITLE_TEXT_OFFSET;

		LinaVG::SDFTextOptions opts;
		opts.font		 = titleFont;
		opts.textScale	 = 1.5f;
		opts.sdfSoftness = 0.02f;

		const float off = 20.0f;

		const Vector2 titleSize = demoDrawer->CalculateTextSize(title.c_str(), opts);
		demoDrawer->DrawTextSDF(title.c_str(), (pos - Vector2(0.0f, off)).AsLVG(), opts, 0.0f, demoDrawOrder);

		LinaVG::TextOptions altOpts;
		altOpts.font		= titleAltFont;
		altOpts.color		= Theme::GetDef().silent2.AsLVG4();
		const Vector2 altSz = demoDrawer->CalculateTextSize(altTitle.c_str(), altOpts);
		demoDrawer->DrawTextNormal(altTitle.c_str(), (pos + Vector2(0.0f, altSz.y + off)).AsLVG(), altOpts, 0.0f, demoDrawOrder);
	}

	void DrawWarning(const Vector<String>& txtItems)
	{
		const Vector2 pos = GetCurrentPos() + WARNING_OFFSET;

		LinaVG::StyleOptions opts;
		opts.color	   = Theme::GetDef().accentError.AsLVG4();
		opts.aaEnabled = true;

		demoDrawer->DrawCircle(pos.AsLVG(), WARNING_RAD, opts, 36.0f, 0.0f, 0.0f, 360.0f, demoDrawOrder);

		opts.thickness = TIMELINE_THICKNESS / 2;
		demoDrawer->DrawLine((pos + Vector2(WARNING_RAD + 25, 0.0f)).AsLVG(), (GetCurrentPos() + TITLE_CIRCLE_OFFSET - Vector2(CIRCLE_RAD + 25.0f, 0.0f)).AsLVG(), opts, LinaVG::LineCapDirection::None, 0.0f, demoDrawOrder);

		LinaVG::SDFTextOptions txt;
		txt.color		= Theme::GetDef().background0.AsLVG4();
		txt.sdfSoftness = 0.02f;
		txt.textScale	= 1.75f;
		txt.font		= titleFont;

		const Vector2 txtSize = demoDrawer->CalculateTextSize("X", txt);
		const Vector2 txtPos  = pos + Vector2(-txtSize.x * 0.5f, txtSize.y * 0.5f);
		demoDrawer->DrawTextSDF("X", txtPos.AsLVG(), txt, 0.0f, demoDrawOrder);

		Vector2 txtStart = pos + Vector2(0.0f, WARNING_RAD + 50.0f);

		LinaVG::TextOptions itemTxt;
		itemTxt.color	  = Theme::GetDef().accentError.AsLVG4();
		itemTxt.font	  = titleAltFont;
		itemTxt.alignment = LinaVG::TextAlignment::Center;

		for (const auto& str : txtItems)
		{
			const Vector2 sz = demoDrawer->CalculateTextSize(str.c_str(), itemTxt);
			demoDrawer->DrawTextNormal(str.c_str(), txtStart.AsLVG(), itemTxt, 0.0f, demoDrawOrder);
			txtStart.y += sz.y + 25.0f;
		}
	}

	void Draw_Intro()
	{
		DrawTitleCircleAnimated();
		DrawTitle("REINVENTING THE WHEEL", "And how not to do it specifically.");
		DrawTimelineLine();
	}

	void Draw_CompsOfEngine()
	{
		DrawTitleCircleAnimated();
		DrawTitle("A GAME ENGINE", "Variety of major/minor modular parts.");
		DrawTimelineLine();
		BulletPointContent({
			"Input Processing",
			"Audio",
			"Rendering",
			"World/Logic",
			"Network",
			"Physics",
			"Resources",
			"Platform/Core",
			"Millions of utilities",
		});
	}

	void Draw_Page1()
	{
		DrawTitleCircleDate("2018", "OCT");
		DrawTitle("BACHELORs THESIS", "Writing an ECS based engine for the thesis.");
		DrawTimelineLine();
		float maxX = BulletPointContent({
			"Traditional way: EC",
			"Entities/Actors/Gameobjects",
			"Components as behaviours",
		});
		BulletPointContent(
			{
				"ECS",
				"Entity-Component-System(s)",
				"Uuuh super cool and fast",
				"Data-oriented",
			},
			maxX + 50);
	}

	void Draw_Page2()
	{
		DrawTitleCircleDate("2019", "JUNE");
		DrawTitle("THESIS DONE", "Basically an ECS cube renderer.");
		DrawTimelineLine();
		DrawImageContent(textureLinaECS);
	}

	void Draw_Page3()
	{
		DrawTitleCircleDate("2020", "DEC");
		DrawTitle("IMPROVING NICELY", "Many many new features and improvements, time to make a game!");
		DrawTimelineLine();
		Vector2 imgSize = DrawImageContent(textureLina7DFPS);
		BulletPointContent(
			{
				"PBR",
				"Lights",
				"Shadows",
				"Reflections",
				"Resources",
				"ENTT",
				"Dear ImGui",
				"Bullet Physics",
				"Input",
				"Utilities",
			},
			imgSize.x + 90.0f);
	}

	void Draw_Page4()
	{
		DrawTitleCircleDate("2021", "DEC");
		DrawTitle("DEPRESSED IMPROVEMENTS", "Nothing major but small fixes here and there, also new editor styling + PhysX.");
		DrawTimelineLine();
		DrawImageContent(textureLinaNewEditor);
	}

	void Draw_Page5()
	{
		DrawTitleCircleDate("2022", "JAN");
		DrawTitle("THE GREAT REWRITE", "Starting a vulkan renderer + new editor.");
		DrawTimelineLine();
		DrawWarning({
			"Editor Rewrite",
			"Renderer Rewrite for Vulkan",
		});
		DrawImageContent(textureLinaDemoEditor, true, 0.8f);
	}

	void Draw_Page6()
	{
		DrawTitleCircleDate("2022", "MAR");
		DrawTitle("A NEW GUI ADVENTURE", "A side quest for couple of months...");
		DrawTimelineLine();
		Vector2 sz = DrawImageContent(textureLinaVG, false, 0.5f);

		auto drawSinBezier = [](const Vector2& pos) {
			LinaVG::StyleOptions	 defaultStyle;
			LinaVG::LineJointType	 jointType;
			LinaVG::LineCapDirection lineCap = LinaVG::LineCapDirection::None;

			static float controlPos1Y = 140;
			static float controlPos2Y = -140;

			controlPos1Y = std::sin(SystemInfo::GetAppTimeF() * 2) * 200;
			controlPos2Y = std::cos(SystemInfo::GetAppTimeF() * 2) * 200;

			jointType					 = LinaVG::LineJointType::Miter;
			defaultStyle.color.start	 = Vector4(0.5f, 1.0f, 1.0f, 1.0f).AsLVG4();
			defaultStyle.color.end		 = Vector4(1.0f, 0.0f, 0.0f, 1.0f).AsLVG4();
			defaultStyle.textureHandle	 = 0;
			defaultStyle.thickness.start = 2.0f;
			defaultStyle.thickness.end	 = 16.0f;
			demoDrawer->DrawBezier(pos.AsLVG(), Vector2(pos.x + 200, pos.y + controlPos1Y).AsLVG(), Vector2(pos.x + 400, pos.y + controlPos2Y).AsLVG(), Vector2(pos.x + 600, pos.y).AsLVG(), defaultStyle, lineCap, jointType, 1, 100);
		};

		drawSinBezier(GetCurrentPos() + PAGE_CONTENTS_OFFSET + Vector2(sz.x + 100, 50.0f));

		{

			LinaVG::StyleOptions defaultStyle;
			defaultStyle.isFilled = true;

			//*************************** ROW 1 ***************************/

			Vector2 startPos = GetCurrentPos() + PAGE_CONTENTS_OFFSET + Vector2(0.0f, sz.y + 100);
			// Filled
			defaultStyle.thickness				  = 5.0f;
			defaultStyle.outlineOptions.color	  = LinaVG::Vec4(0, 0, 0, 1);
			defaultStyle.outlineOptions.thickness = 3.0f;
			demoDrawer->DrawRect(startPos.AsLVG(), LinaVG::Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, 0.0f, demoDrawOrder);

			// Non filled outer
			startPos.x += 200;
			defaultStyle.outlineOptions.color.start = LinaVG::Vec4(1, 0, 0, 1);
			defaultStyle.outlineOptions.color.end	= LinaVG::Vec4(0, 0, 1, 1);
			defaultStyle.isFilled					= false;
			demoDrawer->DrawRect(startPos.AsLVG(), LinaVG::Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, 0.0f, demoDrawOrder);

			// Non filled inner
			startPos.x += 200;
			defaultStyle.outlineOptions.color		  = LinaVG::Vec4(0, 0.5f, 0, 1);
			defaultStyle.outlineOptions.drawDirection = LinaVG::OutlineDrawDirection::Inwards;
			defaultStyle.isFilled					  = false;
			demoDrawer->DrawRect(startPos.AsLVG(), LinaVG::Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, 0.0f, demoDrawOrder);

			// Both
			startPos.x += 200;
			defaultStyle.outlineOptions.color.start	  = LinaVG::Vec4(1, 0, 0, 1);
			defaultStyle.outlineOptions.color.end	  = LinaVG::Vec4(0, 0, 1, 1);
			defaultStyle.outlineOptions.drawDirection = LinaVG::OutlineDrawDirection::Both;
			defaultStyle.isFilled					  = false;
			demoDrawer->DrawRect(startPos.AsLVG(), LinaVG::Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, 0.0f, demoDrawOrder);
		}

		{
			LinaVG::StyleOptions defaultStyle;
			defaultStyle.isFilled = true;
			Vector2 startPos	  = GetCurrentPos() + PAGE_CONTENTS_OFFSET + Vector2(0.0f, sz.y + 100);

			// Horizontal gradient
			startPos.y += 200;
			defaultStyle.rounding	 = 0.0f;
			defaultStyle.color.start = LinaVG::Vec4(1.0f, 0.2f, 0.2f, 1.0f);
			defaultStyle.color.end	 = LinaVG::Vec4(0.2f, 0.2f, 1.0f, 1.0f);
			demoDrawer->DrawNGon(LinaVG::Vec2(startPos.x + 75, startPos.y + 75), 75, 8, defaultStyle, 0.0f, demoDrawOrder);

			// Horizontal gradient.
			startPos.x += 200;
			defaultStyle.rounding	 = 0.0f;
			defaultStyle.color.start = LinaVG::Vec4(0.2f, 0.2f, 1.0f, 1.0f);
			defaultStyle.color.end	 = LinaVG::Vec4(1.0f, 0.2f, 0.2f, 1.0f);
			demoDrawer->DrawRect(startPos.AsLVG(), LinaVG::Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, 0.0f, demoDrawOrder);

			// Vertical gradient
			startPos.x += 200;
			defaultStyle.color.gradientType = LinaVG::GradientType::Vertical;
			defaultStyle.color.start		= LinaVG::Vec4(1.0f, 1.0f, 0.0f, 1.0f);
			defaultStyle.color.end			= LinaVG::Vec4(0.0f, 1.0f, 1.0f, 1.0f);
			demoDrawer->DrawTriangle(LinaVG::Vec2(startPos.x + 75, startPos.y), LinaVG::Vec2(startPos.x + 150, startPos.y + 150), LinaVG::Vec2(startPos.x, startPos.y + 150), defaultStyle, 0.0f, demoDrawOrder);

			// Vertical gradient.
			startPos.x += 200;
			defaultStyle.color.gradientType = LinaVG::GradientType::Vertical;
			defaultStyle.color.start		= LinaVG::Vec4(1.0f, 1.0f, 0.0f, 1.0f);
			defaultStyle.color.end			= LinaVG::Vec4(0.0f, 1.0f, 1.0f, 1.0f);
			demoDrawer->DrawCircle(LinaVG::Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, 0.0f, 0.0f, 360.0f, demoDrawOrder);
		}
	}

	void Draw_Page7()
	{
		DrawTitleCircleDate("2022", "JUN");
		DrawTitle("INTEGRATE LINAVG", "Yet another side quest...");
		DrawTimelineLine();
		BulletPointContent({
			"Tightly coupled systems",
			"Editor needs a lot of refactors",
			"Better to just rewrite",
			"Ditch Dear IMGUI completely",
		});
		DrawWarning({
			"Editor Rewrite for LinaVG",
		});
	}

	void Draw_Page8()
	{
		DrawTitleCircleDate("2023", "FEB");
		DrawTitle("VK BAD DX GOOD", "Just another shiny toy for a kid.");
		DrawTimelineLine();
		Vector2 sz = DrawImageContent(textureDX12, true, 0.5f);
		BulletPointContent(
			{
				"Don't care about non-Windows platforms",
				"Editor doesn't need a rewrite, but let's do it for good measure",
				"DX12 is thy new prodigy",
			},
			sz.x + 100.0f);

		DrawWarning({
			"Editor Rewrite",
			"Renderer Rewrite for DX12",
		});
	}

	void Draw_Page9()
	{
		DrawTitleCircleDate("2023", "JUN");
		DrawTitle("LINAGX", "I think I have a problem with side quests...");
		DrawTimelineLine();
		DrawImageContent(textureLinaGX, true, 0.8f);
		DrawWarning({
			"Renderer Rewrite for LinaGX",
			"Editor Rewrite for good measure",
		});
	}

	void Draw_Page10()
	{
		DrawTitleCircleDate("2023", "OCT");
		DrawTitle("ENOUGH SIDE QUESTS FOR NOW", "Now we are starting the development for real.");
		BulletPointContent({
			"Graphics backend via LinaGX: Vulkan, DX12, Metal",
			"Input processing via LinaGX: Win32, MacOS",
			"Vector graphics via LinaVG",
			"Widget system using LinaVG for editor & in-game",
			"Traditional entity-component system",
			"Serialization/IO/FileSystem code",
			"Memory Management",
			"Platform Toolset/Utilities",
			"Reflection",
			"Profiling Tools",
			"And other bunch...",
		});

		BulletPointContent(
			{
				"Things to do:",
				"Physics :)",
				"Audio stuff",
				"Networking",
				"Bunch of editor tools that will drive me insane",
				"Survive",
			},
			950.0f);
		DrawTimelineLine();
	}

	void Draw_Page11()
	{
		DrawTitleCircleAnimated();
		DrawTitle("LEARNINGS", "(I learned nothing from this. I will keep side questing.)");
		BulletPointContent({
			"Totally reinvent the wheel: amazing learnings!",
			"DO NOT reinvent ALL the parts",
			"Be product oriented, only make something if you are gonna use it immediately",
			"Don't get tricked by shiny tools",
			"Brute-force doing things is as radical as being a perfectionist, find the balance",
			"Don't listen to a guy who hasn't released a game in his engine for 5+ years",
		});

		DrawTimelineLine();
	}

	void Draw_Page12()
	{
		DrawTitleCircleAnimated();
		DrawTitle("WHATS NEXT", "Follow Lina Engine updates to see me never finish this project.");
		BulletPointContent({"https://twitter.com/inanevin0", "https://github.com/inanevin"});

		DrawTimelineLine();
	}

	void PanelWorld::Construct()
	{
		m_wm		 = m_system->CastSubsystem<WorldManager>(SubsystemType::WorldManager);
		m_gfxMan	 = m_system->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
		titleFont	 = m_resourceManager->GetResource<Font>("Resources/Editor/Fonts/DemoTitle.ttf"_hs)->GetLinaVGFont(1.0f);
		titleAltFont = m_resourceManager->GetResource<Font>("Resources/Editor/Fonts/DemoTitleAlt.ttf"_hs)->GetLinaVGFont(1.0f);

		textureLinaECS		  = m_resourceManager->GetResource<Texture>("Resources/Editor/Textures/Demo/LinaECS.png"_hs);
		textureLinaPBR		  = m_resourceManager->GetResource<Texture>("Resources/Editor/Textures/Demo/LinaPBR1.png"_hs);
		textureLinaPBR2		  = m_resourceManager->GetResource<Texture>("Resources/Editor/Textures/Demo/LinaPBR2.png"_hs);
		textureLinaPBR3		  = m_resourceManager->GetResource<Texture>("Resources/Editor/Textures/Demo/LinaPBR3.png"_hs);
		textureLinaDemoEditor = m_resourceManager->GetResource<Texture>("Resources/Editor/Textures/Demo/LinaDummyEditor.png"_hs);
		textureLina7DFPS	  = m_resourceManager->GetResource<Texture>("Resources/Editor/Textures/Demo/sdfps.png"_hs);
		textureLinaNewEditor  = m_resourceManager->GetResource<Texture>("Resources/Editor/Textures/Demo/LinaNewEditor.png"_hs);
		textureLinaVG		  = m_resourceManager->GetResource<Texture>("Resources/Editor/Textures/Demo/LinaVG.png"_hs);
		textureLinaGX		  = m_resourceManager->GetResource<Texture>("Resources/Editor/Textures/Demo/LinaGX.png"_hs);
		textureDX12			  = m_resourceManager->GetResource<Texture>("Resources/Editor/Textures/Demo/dx12.jpg"_hs);
	}

	void PanelWorld::Tick(float delta)
	{
		m_world = m_wm->GetMainWorld();
		if (!m_world)
			return;

		m_worldRenderer = m_wm->GetWorldRenderer(m_world);

		const Vector2ui size = Vector2ui(static_cast<uint32>(Math::CeilToInt(GetSizeX())), static_cast<uint32>(Math::CeilToInt(GetSizeY())));
		m_wm->ResizeWorldTexture(m_world, size);
	}

	void PanelWorld::Draw()
	{
		LinaVG::StyleOptions opts;
		opts.color = Theme::GetDef().background1.AsLVG4();
		m_lvg->DrawRect(m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);

		demoDrawer	  = m_lvg;
		demoDrawOrder = m_drawOrder;

		widgetSize = m_rect.size;
		// originOffset = Vector2(0.0f, -currentPage * widgetSize.y);
		originOffset		= Math::Lerp(originOffset, Vector2(originOffsetTargetX, -currentPage * widgetSize.y), 0.016f * 15.0f);
		currentPagePosition = m_rect.pos;

		m_manager->SetClip(m_rect, {});

		Draw_Intro();
		currentPagePosition.y += widgetSize.y;
		Draw_CompsOfEngine();
		currentPagePosition.y += widgetSize.y;
		Draw_Page1();
		currentPagePosition.y += widgetSize.y;
		Draw_Page2();
		currentPagePosition.y += widgetSize.y;
		Draw_Page3();
		currentPagePosition.y += widgetSize.y;
		Draw_Page4();
		currentPagePosition.y += widgetSize.y;
		Draw_Page5();
		currentPagePosition.y += widgetSize.y;
		Draw_Page6();
		currentPagePosition.y += widgetSize.y;
		Draw_Page7();
		currentPagePosition.y += widgetSize.y;
		Draw_Page8();
		currentPagePosition.y += widgetSize.y;
		Draw_Page9();
		currentPagePosition.y += widgetSize.y;
		Draw_Page10();
		currentPagePosition.y += widgetSize.y;
		Draw_Page11();
		currentPagePosition.y += widgetSize.y;
		Draw_Page12();
		currentPagePosition.y += widgetSize.y;

		LinaVG::TextOptions debugOpts;
		debugOpts.font		  = titleAltFont;
		debugOpts.alignment	  = LinaVG::TextAlignment::Right;
		debugOpts.color		  = Theme::GetDef().silent2.AsLVG4();
		const Vector2 dbgPos1 = m_rect.GetEnd() - Vector2(50, 100);
		const Vector2 dbgPos2 = dbgPos1 + Vector2(0, 50.0f);
		m_lvg->DrawTextNormal("Some debug info to look cool:", dbgPos1.AsLVG(), debugOpts, 0.0f, m_drawOrder);

		const String dbgData = "X: " + TO_STRING(originOffset.x) + " Y: " + TO_STRING(originOffset.y);
		m_lvg->DrawTextNormal(dbgData.c_str(), dbgPos2.AsLVG(), debugOpts, 0.0f, m_drawOrder);

		m_manager->UnsetClip();

		return;

		if (!m_world)
			return;

		m_lvg->DrawImage(m_worldRenderer->GetLightingPassOutput(m_gfxMan->GetLGX()->GetCurrentFrameIndex())->GetBindlessIndex() + 1, m_rect.GetCenter().AsLVG(), m_rect.size.AsLVG(), {1, 1, 1, 1}, 0.0f, m_drawOrder);
	}

	bool PanelWorld::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction action)
	{
		if (action != LinaGX::InputAction::Pressed)
			return false;

		if (keycode == LINAGX_KEY_S)
		{
			currentPage++;
			return true;
		}

		if (keycode == LINAGX_KEY_W)
		{
			if (currentPage > 0)
				currentPage--;
			return true;
		}

		if (keycode == LINAGX_KEY_A)
		{
			originOffsetTargetX += 750.0f;
			originOffsetTargetX = Math::Clamp(originOffsetTargetX, -750.0f, 750.0f);
			return true;
		}

		if (keycode == LINAGX_KEY_D)
		{
			originOffsetTargetX -= 750.0f;
			originOffsetTargetX = Math::Clamp(originOffsetTargetX, -750.0f, 750.0f);
			return true;
		}
	}

} // namespace Lina::Editor
