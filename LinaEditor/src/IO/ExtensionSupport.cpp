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

#include "Editor/IO/ExtensionSupport.hpp"

#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Audio/Audio.hpp"
#include "Core/World/EntityWorld.hpp"

namespace Lina::Editor
{

	TypeID ExtensionSupport::GetTypeIDForExtension(const String& ext)
	{
		if (ext.compare("linaworld") == 0)
			return GetTypeID<EntityWorld>();
		if (ext.compare("linamaterial") == 0)
			return GetTypeID<Material>();
		if (ext.compare("linashader") == 0)
			return GetTypeID<Shader>();
		if (ext.compare("mp3") == 0)
			return GetTypeID<Audio>();
		if (ext.compare("linasampler") == 0)
			return GetTypeID<TextureSampler>();
		if (ext.compare("png") == 0 || ext.compare("jpg") == 0 || ext.compare("jpeg") == 0)
			return GetTypeID<Texture>();
		if (ext.compare("linawidget") == 0)
			return GetTypeID<GUIWidget>();
		if (ext.compare("ttf") == 0 || ext.compare("otf") == 0)
			return GetTypeID<Font>();
		if (ext.compare("gltf") == 0 || ext.compare("glb") == 0)
			return GetTypeID<Model>();

		return 0;
	}

	bool ExtensionSupport::RequiresOutlineFX(TypeID tid)
	{
		if (tid == GetTypeID<Shader>() || tid == GetTypeID<Audio>())
			return false;

		return true;
	}

} // namespace Lina::Editor
