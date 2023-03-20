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

#include "Core/EditorResourcesRegistry.hpp"
#include "Graphics/Resource/Texture.hpp"

namespace Lina::Editor
{
	void EditorResourcesRegistry::RegisterResourceTypes(ResourceManager& rm)
	{
		CoreResourcesRegistry::RegisterResourceTypes(rm);
	}

	Vector<ResourceIdentifier> EditorResourcesRegistry::GetPriorityResources()
	{
		Vector<ResourceIdentifier> resources = CoreResourcesRegistry::GetPriorityResources();

		const uint32 startIndex = static_cast<uint32>(resources.size());

		resources.push_back(ResourceIdentifier("Resources/Editor/Textures/SplashScreen.png", GetTypeID<Texture>(), 0));

		const uint32 totalSize = static_cast<uint32>(resources.size());
		for (uint32 i = startIndex; i < totalSize; i++)
		{
			auto& ident = resources[i];
			ident.sid	= TO_SID(ident.path);
		}

		return resources;
	}

	Vector<Pair<StringID, ResourceMetadata>> EditorResourcesRegistry::GetPriorityResourcesMetadata()
	{
		auto metalist = CoreResourcesRegistry::GetPriorityResourcesMetadata();

		// Add custom

		return metalist;
	}

	Vector<ResourceIdentifier> EditorResourcesRegistry::GetCoreResources()
	{
		Vector<ResourceIdentifier> resources  = CoreResourcesRegistry::GetCoreResources();
		const uint32			   startIndex = static_cast<uint32>(resources.size());

		resources.push_back(ResourceIdentifier("Resources/Editor/Textures/TitleText.png", GetTypeID<Texture>(), 0));

		const String& titleTextStartPath = "Resources/Editor/Textures/TitleTextAnim/lina_title_anim";

		for (int i = 1; i < 21; i++)
		{
			const String& finalPath = titleTextStartPath + TO_STRING(i) + ".png";
			resources.push_back(ResourceIdentifier(finalPath, GetTypeID<Texture>(), 0));
		}

		const uint32 totalSize = static_cast<uint32>(resources.size());
		for (uint32 i = startIndex; i < totalSize; i++)
		{
			auto& ident = resources[i];
			ident.sid	= TO_SID(ident.path);
		}

		return resources;
	}

	Vector<Pair<StringID, ResourceMetadata>> EditorResourcesRegistry::GetCoreResourcesMetadata()
	{
		auto metalist = CoreResourcesRegistry::GetCoreResourcesMetadata();
		// Add custom
		return metalist;
	}

} // namespace Lina::Editor