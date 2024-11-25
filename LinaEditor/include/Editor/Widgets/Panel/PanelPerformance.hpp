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

#include "Editor/Widgets/Panel/Panel.hpp"
#include "Core/Resources/ResourceManagerListener.hpp"
#include "Common/Data/Deque.hpp"

namespace Lina
{
	class Text;
	class DirectionalLayout;
	class Circle;
} // namespace Lina
namespace Lina::Editor
{
	class Editor;
	class Table;
	class IconTabs;
	class LineGraph;

	class PanelPerformance : public Panel, public ResourceManagerListener
	{
	public:
		enum class ResourcesSort : uint8
		{
			Name,
			ID,
			Type,
			Size,
		};

		PanelPerformance() : Panel(PanelType::Performance){};
		virtual ~PanelPerformance() = default;

		virtual void Construct() override;
		virtual void Destruct() override;
		virtual void PreTick() override;
		virtual void Tick(float delta) override;
		virtual void OnResourceManagerPreDestroyHW(const HashSet<Resource*>& resources) override;
		virtual void OnResourceManagerGeneratedHW(const HashSet<Resource*>& resources) override;
		virtual void SaveLayoutToStream(OStream& stream) override;
		virtual void LoadLayoutFromStream(IStream& stream) override;

	private:
		void SelectContent(uint8 idx);
		void BuildContentsProfiling();
		void BuildContentsResources();
		void RefreshResourcesTable();

	private:
		struct ProfilingData
		{
			LineGraph*	 fpsGraph = nullptr;
			Deque<float> fpsFrames;
			Table*		 infoTable	   = nullptr;
			Circle*		 cpuUtilCircle = nullptr;
			Circle*		 memCircle	   = nullptr;
			Circle*		 memPeakCircle = nullptr;
			Text*		 cpuUtilText   = nullptr;
			Text*		 memText	   = nullptr;
			Text*		 memPeakText   = nullptr;
			Text*		 tris		   = nullptr;
			Text*		 vertices	   = nullptr;
			Text*		 indices	   = nullptr;
			Text*		 drawCalls	   = nullptr;
			Text*		 dpi		   = nullptr;
			Text*		 delta		   = nullptr;
			Text*		 smoothedDelta = nullptr;

			uint32 _tris	= 0;
			uint32 _verts	= 0;
			uint32 _indices = 0;
			uint32 _dc		= 0;
		};

		struct ResourcesData
		{
			ResourcesSort sort		= ResourcesSort::Name;
			Table*		  table		= nullptr;
			String		  searchStr = "";
		};

	private:
		Editor*			   m_editor			= nullptr;
		uint8			   m_currentContent = 0;
		DirectionalLayout* m_layout			= nullptr;
		IconTabs*		   m_iconTabs		= nullptr;
		ResourcesData	   m_resourcesData	= {};
		ProfilingData	   m_profilingData	= {};
	};

	LINA_WIDGET_BEGIN(PanelPerformance, Hidden)
	LINA_CLASS_END(PanelPerformance)

} // namespace Lina::Editor
