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
#include "Common/Math/Math.hpp"
#include "Common/Math/Math.hpp"
#include "Core/Graphics/Utility/TextureAtlas.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Platform/LinaGXIncl.hpp"

namespace Lina::Editor
{
	TextureAtlasImage* AtlasManager::AddImageToAtlas(uint8* data, const Vector2ui& size, LinaGX::Format format)
	{
		LOCK_GUARD(m_mtx);
		TextureAtlasImage* rect = nullptr;
		for (TextureAtlas* atlas : m_atlasPool)
		{
			if (atlas->GetFormat() != format)
				continue;

			rect = atlas->AddImage(data, size);
			if (rect != nullptr)
				return rect;
		}

		if (rect == nullptr)
		{
			const String	atlasName = "AtlasManagerAtlas_" + TO_STRING(m_atlasPool.size());
			const Vector2ui atlasSize = Vector2ui(Math::Max((uint32)1024, size.x), Math::Max((uint32)1024, size.y));
			TextureAtlas*	atlas	  = new TextureAtlas(&m_editor->GetResourceManagerV2(), atlasSize, format);
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

	TextureAtlas* AtlasManager::AddCustomAtlas(const String& baseFolder, StringID sid, const Vector2ui& size)
	{
		TextureAtlas* atlas	 = new TextureAtlas(&m_editor->GetResourceManagerV2(), size, LinaGX::Format::R8G8B8A8_SRGB);
		m_customAtlases[sid] = atlas;

		Vector<String> files;
		FileSystem::GetFilesInDirectory(baseFolder, files);

		for (const String& file : files)
		{
			LinaGX::TextureBuffer buffer;
			LinaGX::LoadImageFromFile(file.c_str(), buffer, 4);

			if (buffer.pixels == nullptr)
				continue;

			const String filename = FileSystem::GetFilenameOnlyFromPath(file);
			atlas->AddImage(buffer.pixels, Vector2ui(buffer.width, buffer.height), TO_SID(filename));
			LinaGX::FreeImage(buffer.pixels);
		}

		atlas->RefreshSW();
		return atlas;
	}

	void AtlasManager::Initialize(Editor* editor)
	{
		m_editor = editor;
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

	void AtlasManager::RefreshPoolAtlases()
	{
		for (TextureAtlas* atlas : m_atlasPool)
		{
			atlas->RefreshSW();
			atlas->RefreshGPU(m_editor->GetEditorRenderer().GetUploadQueue());
		}

		m_editor->GetEditorRenderer().MarkBindlessDirty();
	}

} // namespace Lina::Editor
