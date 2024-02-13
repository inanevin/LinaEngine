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

#include "FlarePackerAppDelegate.hpp"
#include "FlarePackerCommon.hpp"
#include "Core/Application.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Font.hpp"

namespace Lina
{

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

	void FlarePackerAppDelegate::RegisterAppResources(ResourceManager& rm)
	{
		ApplicationDelegate::RegisterAppResources(rm);

		Vector<ResourceIdentifier> resources;
		resources.push_back(ResourceIdentifier(FLP_ICON_FONT_PATH, GetTypeID<Font>(), 0, true, ResourceTag::Core));

		for (auto& r : resources)
			r.sid = TO_SID(r.path);

		rm.RegisterAppResources(resources);
	}

	bool FlarePackerAppDelegate::FillResourceCustomMeta(StringID sid, OStream& stream)
	{
		if (ApplicationDelegate::FillResourceCustomMeta(sid, stream))
			return true;

		if (sid == FLP_ICON_FONT_SID)
		{
			Font::Metadata customMeta = {
				.points = {{.size = 12, .dpiLimit = 1.1f}, {.size = 14, .dpiLimit = 1.15f}, {.size = 16, .dpiLimit = 1.35f}, {.size = 20, .dpiLimit = 10.0f}},
				.isSDF	= true,
			};
			customMeta.SaveToStream(stream);
			return true;
		}

		return false;
	}

	void FlarePackerAppDelegate::OnPreInitialize()
	{
		m_flarePacker.PreInitialize(m_app);
	}

	void FlarePackerAppDelegate::OnInitialize()
	{
		m_flarePacker.Initialize();
	}


} // namespace Lina
