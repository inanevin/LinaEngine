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
#include "Graphics/Resource/Font.hpp"
#include "Graphics/Core/CommonGraphics.hpp"

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
		auto list = CoreResourcesRegistry::GetPriorityResourcesMetadata();

		// Add custom
		ResourceMetadata metadata;
		metadata.SetSID("Sampler"_hs, DEFAULT_GUI_SAMPLER_SID);
		list.push_back(linatl::make_pair("Resources/Editor/Textures/SplashScreen.png"_hs, metadata));
		metadata.ClearAll();

		return list;
	}

	Vector<ResourceIdentifier> EditorResourcesRegistry::GetCoreResources()
	{
		Vector<ResourceIdentifier> resources  = CoreResourcesRegistry::GetCoreResources();
		const uint32			   startIndex = static_cast<uint32>(resources.size());

		resources.push_back(ResourceIdentifier("Resources/Editor/Textures/TitleTextAnimation.png", GetTypeID<Texture>(), 0));
		resources.push_back(ResourceIdentifier("Resources/Editor/Textures/EditorImages.png", GetTypeID<Texture>(), 0));
		resources.push_back(ResourceIdentifier("Resources/Editor/Fonts/EditorIcons_1x.ttf", GetTypeID<Font>(), 0));
		resources.push_back(ResourceIdentifier("Resources/Editor/Fonts/EditorIcons_2x.ttf", GetTypeID<Font>(), 0));
		resources.push_back(ResourceIdentifier("Resources/Editor/Fonts/NunitoSansBoldBig_1x.ttf", GetTypeID<Font>(), 0));
		resources.push_back(ResourceIdentifier("Resources/Editor/Fonts/NunitoSansBoldBig_2x.ttf", GetTypeID<Font>(), 0));
		resources.push_back(ResourceIdentifier("Resources/Editor/Fonts/NunitoSansBoldBig_3x.ttf", GetTypeID<Font>(), 0));
		resources.push_back(ResourceIdentifier("Resources/Editor/Fonts/NunitoSansBoldBig_4x.ttf", GetTypeID<Font>(), 0));

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
		auto			 list = CoreResourcesRegistry::GetCoreResourcesMetadata();
		ResourceMetadata metadata;

		// -------------
		metadata.SetInt("Size"_hs, 16);
		metadata.SetBool("IsSDF"_hs, true);
		metadata.SetInt("CustomGlyphRanges"_hs, 2);
		metadata.SetInt("Range_0"_hs, 0x07F);
		metadata.SetInt("Range_1"_hs, 0x0FF);
		list.push_back(linatl::make_pair("Resources/Editor/Fonts/EditorIcons_1x.ttf"_hs, metadata));
		metadata.ClearAll();

		// -------------
		metadata.SetInt("Size"_hs, 22);
		metadata.SetBool("IsSDF"_hs, true);
		metadata.SetInt("CustomGlyphRanges"_hs, 2);
		metadata.SetInt("Range_0"_hs, 0x07F);
		metadata.SetInt("Range_1"_hs, 0x0FF);
		list.push_back(linatl::make_pair("Resources/Editor/Fonts/EditorIcons_2x.ttf"_hs, metadata));
		metadata.ClearAll();

		//-------------
		metadata.SetBool("IsSDF"_hs, false);
		metadata.SetInt("Size"_hs, 32);
		list.push_back(linatl::make_pair("Resources/Editor/Fonts/NunitoSansBoldBig_1x.ttf"_hs, metadata));
		metadata.ClearAll();

		//-------------
		metadata.SetBool("IsSDF"_hs, false);
		metadata.SetInt("Size"_hs, 36);
		list.push_back(linatl::make_pair("Resources/Editor/Fonts/NunitoSansBoldBig_2x.ttf"_hs, metadata));
		metadata.ClearAll();

		//-------------
		metadata.SetBool("IsSDF"_hs, false);
		metadata.SetInt("Size"_hs, 40);
		list.push_back(linatl::make_pair("Resources/Editor/Fonts/NunitoSansBoldBig_3x.ttf"_hs, metadata));
		metadata.ClearAll();

		//-------------
		metadata.SetBool("IsSDF"_hs, false);
		metadata.SetInt("Size"_hs, 44);
		list.push_back(linatl::make_pair("Resources/Editor/Fonts/NunitoSansBoldBig_4x.ttf"_hs, metadata));
		metadata.ClearAll();

		// -------------
		metadata.SetSID("Sampler"_hs, DEFAULT_GUI_SAMPLER_SID);
		list.push_back(linatl::make_pair("Resources/Editor/Textures/EditorImages.png"_hs, metadata));
		metadata.ClearAll();

		// Add custom
		return list;
	}

} // namespace Lina::Editor