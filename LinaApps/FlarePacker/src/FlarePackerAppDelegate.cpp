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

namespace Lina
{
	class Widget
	{
	public:
		void Render(){};

		Vector2i position = Vector2i::Zero;
		Vector2i size	  = Vector2i::Zero;
	};

	Widget* emptyWidget;

	class FreeRoam : public Widget
	{
	public:
		Widget* child = emptyWidget;

		void Render(){

		};

		void operator[](const FreeRoam& copy)
		{
		}
	};

	class FileMenu : public Widget
	{
	};

	class Column : public Widget
	{
	};

	template <typename T> Widget* WidgetAlloc(T t)
	{
		return new T();
	}

	SystemInitializationInfo Lina_GetInitInfo()
	{
		return SystemInitializationInfo{
			.appName			 = "Flare Packer",
			.windowWidth		 = 800,
			.windowHeight		 = 600,
			.windowStyle		 = LinaGX::WindowStyle::BorderlessApplication,
			.appListener		 = new Lina::FlarePackerAppDelegate(),
			.resourceManagerMode = Lina::ResourceManagerMode::File,
		};
	}

	void FlarePackerAppDelegate::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
	}

	void FlarePackerAppDelegate::RenderSurfaceOverlay(LinaGX::CommandStream* cmdStream, LinaGX::Window* window, int32 threadIndex)
	{
		Widget* fr = WidgetAlloc<FreeRoam>({
			.size  = window->GetSize(),
			.child = WidgetAlloc<Column>({

			}),
		});

		fr->Render();
	}

} // namespace Lina
