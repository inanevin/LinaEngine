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

#include "Editor/Atlas/AtlasManager.hpp"
#include "Editor/Editor.hpp"
#include "Common/System/System.hpp"
#include "Common/Math/Math.hpp"
#include "Core/Graphics/Utility/TextureAtlas.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Platform/LinaGXIncl.hpp"

namespace Lina::Editor
{
	TextureAtlasImage* AtlasManager::AddImageToAtlas(uint8* data, const Vector2ui& size, uint32 bytesPerPixel)
	{
		LOCK_GUARD(m_mtx);
		TextureAtlasImage* rect = nullptr;
		for (TextureAtlas* atlas : m_atlasPool)
		{
			if (atlas->GetBytesPerPixel() != bytesPerPixel)
				continue;

			rect = atlas->AddImage(data, size);
			if (rect != nullptr)
				return rect;
		}

		if (rect == nullptr)
		{
			LinaGX::Format format = LinaGX::Format::R8_UNORM;

			if (bytesPerPixel == 1)
				format = LinaGX::Format::R8_UNORM;
			else if (bytesPerPixel == 2)
				format = LinaGX::Format::R8G8_SNORM;
			else if (bytesPerPixel == 4)
				format = LinaGX::Format::R8G8B8A8_SRGB;
			else if (bytesPerPixel == 8)
				format = LinaGX::Format::R16G16B16A16_UNORM;
			else
			{
				LINA_ASSERT(false, "");
			}

			const String	atlasName = "AtlasManagerAtlas_" + TO_STRING(m_atlasPool.size());
			const Vector2ui atlasSize = Vector2ui(Math::Max((uint32)1024, size.x), Math::Max((uint32)1024, size.y));
			TextureAtlas*	atlas	  = new TextureAtlas(TO_SID(atlasName), &m_editor->GetResourceManagerV2(), atlasSize, bytesPerPixel, format);
			m_atlasPool.push_back(atlas);
			rect = atlas->AddImage(data, size);
			LINA_ASSERT(rect != nullptr, "");
			return rect;
		}

		LINA_ERR("Something went wrong with atlas management!");
		return nullptr;
	}

	void AtlasManager::RemoveImage(TextureAtlasImage* rect)
	{
		LOCK_GUARD(m_mtx);
		for (TextureAtlas* atlas : m_atlasPool)
		{
			if (atlas->RemoveImage(rect))
				break;
		}
	}

	void AtlasManager::Initialize(Editor* editor)
	{
		m_editor = editor;

		auto addAtlas = [&](const String& directory, StringID sid, const Vector2ui& sz) -> TextureAtlas* {
			TextureAtlas* atlas	 = new TextureAtlas(sid, &m_editor->GetResourceManagerV2(), sz, 4, LinaGX::Format::R8G8B8A8_SRGB);
			m_customAtlases[sid] = atlas;

			Vector<String> files;
			FileSystem::GetFilesInDirectory(directory, files);

			for (const String& file : files)
			{
				LinaGX::TextureBuffer buffer;
				LinaGX::LoadImageFromFile(file.c_str(), buffer);

				if (buffer.pixels == nullptr)
					continue;

				const String filename = FileSystem::GetFilenameOnlyFromPath(file);
				atlas->AddImage(buffer.pixels, Vector2ui(buffer.width, buffer.height), TO_SID(filename));
				LinaGX::FreeImage(buffer.pixels);
			}

			return atlas;
		};

		addAtlas("Resources/Editor/Textures/Atlas/MiscTextures/", "MiscTextures"_hs, Vector2ui(1024, 1024));
		addAtlas("Resources/Editor/Textures/Atlas/ProjectIcons/", "ProjectIcons"_hs, Vector2ui(2048, 2048));
		RefreshDirtyAtlases();
	}

	TextureAtlasImage* AtlasManager::GetImageFromAtlas(StringID atlasSID, StringID image)
	{
		TextureAtlas* atlas = m_customAtlases.at(atlasSID);
		return atlas->GetImage(image);
	}

	void AtlasManager::Shutdown()
	{
		for (TextureAtlas* atlas : m_atlasPool)
			delete atlas;
		m_atlasPool.clear();

		for (const Pair<StringID, TextureAtlas*>& p : m_customAtlases)
		{
			delete p.second;
		}

		m_customAtlases.clear();
	}

	void AtlasManager::RefreshDirtyAtlases()
	{
		for (TextureAtlas* atlas : m_atlasPool)
			atlas->RefreshGPU(m_editor->GetEditorRenderer().GetUploadQueue());

		for (Pair<StringID, TextureAtlas*> pair : m_customAtlases)
			pair.second->RefreshGPU(m_editor->GetEditorRenderer().GetUploadQueue());

		m_editor->GetEditorRenderer().MarkBindlessDirty();
	}
} // namespace Lina::Editor
