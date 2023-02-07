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

#include "Core/CoreResourcesRegistry.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "Graphics/Resource/Model.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Resource/Font.hpp"
#include "Graphics/Resource/Shader.hpp"
#include "Graphics/Resource/Material.hpp"

namespace Lina
{
	Vector<StringID> CoreResourcesRegistry::s_coreShaders = {
		 "Resources/Core/Shaders/LitStandard.linashader"_hs,
		 "Resources/Core/Shaders/GUIStandard.linashader"_hs,
		 "Resources/Core/Shaders/GUIText.linashader"_hs,
		 "Resources/Core/Shaders/ScreenQuads/SQFinal.linashader"_hs,
		 "Resources/Core/Shaders/ScreenQuads/SQPostProcess.linashader"_hs,
	};

	void CoreResourcesRegistry::RegisterResourceTypes(ResourceManager& rm)
	{
		rm.RegisterResourceType<Model>(50, {"fbx"}, PackageType::Package1);
		rm.RegisterResourceType<Shader>(25, {"linashader"}, PackageType::Package1);
		rm.RegisterResourceType<Texture>(100, {"png", "jpeg", "jpg"}, PackageType::Package1);
		rm.RegisterResourceType<Font>(10, {"ttf", "otf"}, PackageType::Package1);
		rm.RegisterResourceType<Material>(25, {"linamaterial"}, PackageType::Package1);
	}

	Vector<ResourceIdentifier> CoreResourcesRegistry::GetCoreResources()
	{
		Vector<ResourceIdentifier> list;

		// list.push_back(ResourceIdentifier("Resources/Core/Fonts/NunitoSans.ttf", GetTypeID<Font>(), 0));
		// list.push_back(ResourceIdentifier("Resources/Core/Fonts/Rubik-Regular.ttf", GetTypeID<Font>(), 0));
		list.push_back(ResourceIdentifier("Resources/Core/Models/Capsule.fbx", GetTypeID<Model>(), 0));
		list.push_back(ResourceIdentifier("Resources/Core/Models/Cube.fbx", GetTypeID<Model>(), 0));
		list.push_back(ResourceIdentifier("Resources/Core/Models/Cylinder.fbx", GetTypeID<Model>(), 0));
		list.push_back(ResourceIdentifier("Resources/Core/Models/LinaLogo.fbx", GetTypeID<Model>(), 0));
		list.push_back(ResourceIdentifier("Resources/Core/Models/Plane.fbx", GetTypeID<Model>(), 0));
		list.push_back(ResourceIdentifier("Resources/Core/Models/Quad.fbx", GetTypeID<Model>(), 0));
		list.push_back(ResourceIdentifier("Resources/Core/Models/Sphere.fbx", GetTypeID<Model>(), 0));
		list.push_back(ResourceIdentifier("Resources/Core/Textures/DummyBlack_32.png", GetTypeID<Texture>(), 0));
		list.push_back(ResourceIdentifier("Resources/Core/Textures/Grid512.png", GetTypeID<Texture>(), 0));
		list.push_back(ResourceIdentifier("Resources/Core/Textures/Logo_Colored_1024.png", GetTypeID<Texture>(), 0));
		list.push_back(ResourceIdentifier("Resources/Core/Textures/Logo_White_512.png", GetTypeID<Texture>(), 0));
		list.push_back(ResourceIdentifier("Resources/Core/Textures/LogoWithText.png", GetTypeID<Texture>(), 0));

#ifdef LINA_GRAPHICS_VULKAN
		list.push_back(ResourceIdentifier("Resources/Core/Shaders/GUIStandard.linashader", GetTypeID<Shader>(), 0));
		list.push_back(ResourceIdentifier("Resources/Core/Shaders/GUIText.linashader", GetTypeID<Shader>(), 0));
		list.push_back(ResourceIdentifier("Resources/Core/Shaders/LitStandard.linashader", GetTypeID<Shader>(), 0));
		list.push_back(ResourceIdentifier("Resources/Core/Shaders/ScreenQuads/SQFinal.linashader", GetTypeID<Shader>(), 0));
		list.push_back(ResourceIdentifier("Resources/Core/Shaders/ScreenQuads/SQPostProcess.linashader", GetTypeID<Shader>(), 0));
#else
		LINA_NOTIMPLEMENTED;
#endif

		for (auto& ident : list)
			ident.sid = TO_SID(ident.path);

		return list;
	}

	Vector<Pair<StringID, ResourceMetadata>> CoreResourcesRegistry::GetCoreResourceDefaultMetadata()
	{
		Vector<Pair<StringID, ResourceMetadata>> list;
		ResourceMetadata						 metadata;

		//-------------
		metadata.SetInt("VariationCount"_hs, 1);
		metadata.SetInt("Size0"_hs, 12);
		metadata.SetInt("IsSDF0"_hs, false);
		list.push_back(linatl::make_pair("Resources/Core/Fonts/NunitoSans.ttf"_hs, metadata));
		metadata.ClearAll();

		//-------------
		metadata.SetInt("VariationCount"_hs, 2);
		metadata.SetInt("Size0"_hs, 12);
		metadata.SetInt("Size1"_hs, 13);
		metadata.SetInt("IsSDF0"_hs, false);
		metadata.SetInt("IsSDF1"_hs, false);
		list.push_back(linatl::make_pair("Resources/Core/Fonts/Rubik-Regular.ttf"_hs, metadata));
		metadata.ClearAll();

		//-------------

		return list;
	}
} // namespace Lina
