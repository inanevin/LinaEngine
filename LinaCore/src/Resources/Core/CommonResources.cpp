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

#include "Resources/Core/CommonResources.hpp"
#include "Serialization/StringSerialization.hpp"

namespace Lina
{
	Vector<ResourceIdentifier> GCoreResources = {
		ResourceIdentifier("Resources/Core/Fonts/NunitoSans.ttf", GetTypeID<int>(), "Resources/Core/Fonts/NunitoSans.ttf"_hs),
		ResourceIdentifier("Resources/Core/Fonts/Rubik-Regular.ttf", GetTypeID<int>(), "Resources/Core/Fonts/Rubik-Regular.ttf"_hs),
		ResourceIdentifier("Resources/Core/Models/Capsule.fbx", GetTypeID<int>(), "Resources/Core/Models/Capsule.fbx"_hs),
		ResourceIdentifier("Resources/Core/Models/Cube.fbx", GetTypeID<int>(), "Resources/Core/Models/Cube.fbx"_hs),
		ResourceIdentifier("Resources/Core/Models/Cylinder.fbx", GetTypeID<int>(), "Resources/Core/Models/Cylinder.fbx"_hs),
		ResourceIdentifier("Resources/Core/Models/LinaLogo.fbx", GetTypeID<int>(), "Resources/Core/Models/LinaLogo.fbx"_hs),
		ResourceIdentifier("Resources/Core/Models/Plane.fbx", GetTypeID<int>(), "Resources/Core/Models/Plane.fbx"_hs),
		ResourceIdentifier("Resources/Core/Models/Quad.fbx", GetTypeID<int>(), "Resources/Core/Models/Quad.fbx"_hs),
		ResourceIdentifier("Resources/Core/Models/Sphere.fbx", GetTypeID<int>(), "Resources/Core/Models/Sphere.fbx"_hs),
		ResourceIdentifier("Resources/Core/Shaders/GUIStandard.linashader", GetTypeID<int>(), "Resources/Core/Shaders/GUIStandard.linashader"_hs),
		ResourceIdentifier("Resources/Core/Shaders/GUIText.linashader", GetTypeID<int>(), "Resources/Core/Shaders/GUIText.linashader"_hs),
		ResourceIdentifier("Resources/Core/Shaders/LitStandard.linashader", GetTypeID<int>(), "Resources/Core/Shaders/LitStandard.linashader"_hs),
		ResourceIdentifier("Resources/Core/Textures/Grid512.png", GetTypeID<int>(), "Resources/Core/Shaders/LitStandard.linashader"_hs),
		ResourceIdentifier("Resources/Core/Textures/Logo_Colored_1024.png", GetTypeID<int>(), "Resources/Core/Textures/Logo_Colored_1024.png"_hs),
		ResourceIdentifier("Resources/Core/Textures/Logo_White_512.png", GetTypeID<int>(), "Resources/Core/Textures/Logo_White_512.png"_hs),
		ResourceIdentifier("Resources/Core/Textures/LogoWithText.png", GetTypeID<int>(), "Resources/Core/Textures/LogoWithText.png"_hs),
	};

	String GGetPackagePath(PackageType pt)
	{
		switch (pt)
		{
		case PackageType::Static:
			return "Resources/Packages/res_pack_static.linapackage";
		case PackageType::Package1:
			return "Resources/Packages/res_pack_01.linapackage";
		case PackageType::Package2:
			return "Resources/Packages/res_pack_02.linapackage";
		}

		return "";
	}

	void ResourceIdentifier::SaveToStream(OStream& stream)
	{
		StringSerialization::SaveToStream(stream, path);
		stream << sid;
		stream << tid;
	}

	void ResourceIdentifier::LoadFromStream(IStream& stream)
	{
		StringSerialization::LoadFromStream(stream, path);
		stream >> sid;
		stream >> tid;
	}
} // namespace Lina
