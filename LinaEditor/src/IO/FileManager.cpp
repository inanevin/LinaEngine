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

#include "Editor/IO/FileManager.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Meta/ProjectData.hpp"
#include "Common/System/System.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/FileSystem/FileWatcher.hpp"
#include "Common/System/System.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"
#include "Core/World/EntityWorld.hpp"

namespace Lina::Editor
{

	void FileManager::Initialize(Editor* editor)
	{
		m_editor = editor;
	}

	void FileManager::Shutdown()
	{
		ClearResources();
	}

	void FileManager::ClearResources()
	{
		if (m_root != nullptr)
			DeallocItem(m_root);
		m_root = nullptr;
	}

	void FileManager::DeallocItem(DirectoryItem* item)
	{
		for (auto* i : item->children)
			DeallocItem(i);

		if (item->thumbnail != nullptr)
		{
			auto* rm = m_editor->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
			rm->DestroyUserResource(item->thumbnail);
			item->thumbnail = nullptr;
		}

		item->children.clear();
		delete item;
		// m_allocatorPool.Free(item);
	}

	void FileManager::ScanItem(DirectoryItem* item)
	{
		LINA_ASSERT(item->children.size() == 0, "");
		Vector<String> dirs = {};
		FileSystem::GetFilesAndFoldersInDirectory(item->absolutePath, dirs);

		for (const auto& str : dirs)
		{
			const bool isDirectory = FileSystem::IsDirectory(str);
			String	   extension   = "";
			TypeID	   tid		   = 0;

			if (!isDirectory)
			{
				extension = FileSystem::GetFileExtension(str);

				const auto& resourceCaches = m_editor->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager)->GetCaches();

				for (const auto& [typeID, cache] : resourceCaches)
				{
					if (cache->DoesSupportExtension(extension))
					{
						tid = typeID;
						break;
					}
				}

				if (tid == 0)
					continue;
			}

			// DirectoryItem* subItem = new (m_allocatorPool.Allocate(sizeof(DirectoryItem))) DirectoryItem();
			DirectoryItem* subItem = new DirectoryItem();

			subItem->extension	 = extension;
			subItem->tid		 = tid;
			subItem->isDirectory = isDirectory;
			subItem->parent		 = item;
			item->children.push_back(subItem);

			FillPathInformation(subItem, str);

			GenerateThumbnailForItem(subItem);

			if (subItem->isDirectory)
				ScanItem(subItem);
		}

		// Sort alphabetically

		Vector<DirectoryItem*> folders = {}, files = {};
		for (auto* c : item->children)
		{
			if (c->isDirectory)
				folders.push_back(c);
			else
				files.push_back(c);
		}
		linatl::sort(folders.begin(), folders.end(), [](const DirectoryItem* a, const DirectoryItem* b) { return a->folderName < b->folderName; });
		linatl::sort(files.begin(), files.end(), [](const DirectoryItem* a, const DirectoryItem* b) { return a->fileName < b->fileName; });

		item->children.clear();
		item->children.insert(item->children.end(), folders.begin(), folders.end());
		item->children.insert(item->children.end(), files.begin(), files.end());
	}

	void FileManager::RefreshResources()
	{
		ClearResources();

		const String resDir = m_projectDirectory + ""
												   "Resources/";
		if (!FileSystem::FileOrPathExists(resDir))
			return;

		// m_root				= new (m_allocatorPool.Allocate(sizeof(DirectoryItem))) DirectoryItem();
		m_root				= new DirectoryItem();
		m_root->isDirectory = true;
		FillPathInformation(m_root, resDir);
		ScanItem(m_root);
	}

	void FileManager::ClearDirectory(DirectoryItem* item)
	{
		for (auto* c : item->children)
			DeallocItem(c);

		item->children.clear();
	}

	void FileManager::RefreshDirectory(DirectoryItem* item)
	{
		ClearDirectory(item);
		ScanItem(item);
	}

	void FileManager::UpdateItem(DirectoryItem* item, const String& newPath, bool regenerateThumbnail)
	{
		item->absolutePath = newPath;

		if (item->thumbnail && regenerateThumbnail)
		{
			GenerateThumbnailForItem(item);
		}
	}

	void FileManager::GenerateThumbnailForItem(DirectoryItem* item)
	{
		if (item->isDirectory)
			return;

		auto* rm = m_editor->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);

		if (item->thumbnail)
		{
			rm->DestroyUserResource(item->thumbnail);
			item->thumbnail = nullptr;
		}

		const String thumbnailPath = "ResourceThumbnail_" + TO_STRING(item->sid);

		if (item->tid == GetTypeID<Texture>())
		{
			item->thumbnail = rm->CreateUserResource<Texture>(thumbnailPath, TO_SID(thumbnailPath));
			GenerateThumbTexture(item);
		}
		else if (item->tid == GetTypeID<Font>())
		{
			item->thumbnail = rm->CreateUserResource<Texture>(thumbnailPath, TO_SID(thumbnailPath));
			GenerateThumbFont(item);
		}
		else if (item->tid == GetTypeID<Model>())
		{
			item->thumbnail = rm->CreateUserResource<Texture>(thumbnailPath, TO_SID(thumbnailPath));
			GenerateThumbModel(item);
		}
	}

	void FileManager::GenerateThumbTexture(DirectoryItem* item)
	{
		Taskflow tf;
		tf.emplace([item]() {
			LinaGX::TextureBuffer image;
			LinaGX::LoadImageFromFile(item->absolutePath.c_str(), image);

			if (image.pixels != nullptr)
			{
				const float max	   = static_cast<float>(Math::Max(image.width, image.height));
				const float min	   = static_cast<float>(Math::Min(image.width, image.height));
				const float aspect = max / min;

				uint32 width  = RESOURCE_THUMBNAIL_SIZE;
				uint32 height = RESOURCE_THUMBNAIL_SIZE;

				if (image.width > image.height)
					height = static_cast<uint32>(static_cast<float>(width) / aspect);
				else
					width = static_cast<uint32>(static_cast<float>(height) / aspect);

				LinaGX::TextureBuffer resizedBuffer = {
					.pixels		   = new uint8[width * height * image.bytesPerPixel],
					.width		   = width,
					.height		   = height,
					.bytesPerPixel = image.bytesPerPixel,
				};

				if (LinaGX::ResizeBuffer(image, resizedBuffer, width, height, LinaGX::MipmapFilter::Default, LinaGX::ImageChannelMask::RGBA, true))
				{
					item->thumbnail->SetCustomData(
						resizedBuffer.pixels, resizedBuffer.width, resizedBuffer.height, resizedBuffer.bytesPerPixel, LinaGX::ImageChannelMask::RGBA, image.bytesPerPixel == 4 ? LinaGX::Format::R8G8B8A8_SRGB : LinaGX::Format::R16G16B16A16_UNORM);
					delete[] resizedBuffer.pixels;
				}
				else
				{
					LINA_ERR("FileManager: Failed resizing image for thumbnail!");
				}

				LinaGX::FreeImage(image.pixels);
			}
			else
			{
				LINA_ERR("FileManager: Failed loading image for thumbnail!");
			}
		});

		m_editor->GetSystem()->GetMainExecutor()->RunMove(tf);
	}

	void FileManager::GenerateThumbFont(DirectoryItem* item)
	{
		Taskflow tf;

		auto loadGlyph = [](FT_Face face, uint32 code, uint32& width, uint32& height) -> unsigned char* {
			auto index = FT_Get_Char_Index(face, code);

			if (index == 0)
			{
				LINA_ERR("FileManager: Failed finding font char index for thumbnail!");
				return nullptr;
			}

			int err = FT_Load_Glyph(face, index, FT_LOAD_DEFAULT);
			if (err)
			{
				LINA_ERR("FileManager: Failed loading font glyph for thumbnail!");
				return nullptr;
			}

			FT_GlyphSlot slot = face->glyph;
			err				  = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);

			if (err)
			{
				LINA_ERR("FileManager: Failed rendering font glyph for thumbnail!");
				return nullptr;
			}

			const unsigned int glyphWidth = slot->bitmap.width;
			const unsigned int glyphRows  = slot->bitmap.rows;
			const size_t	   bufSize	  = static_cast<size_t>(glyphWidth * glyphRows);

			width  = glyphWidth;
			height = glyphRows;

			unsigned char* buffer = nullptr;
			if (slot->bitmap.buffer != nullptr)
			{
				buffer = (unsigned char*)MALLOC(bufSize);

				if (buffer != 0)
					MEMCPY(buffer, slot->bitmap.buffer, bufSize);
			}

			return buffer;
		};

		tf.emplace([item, loadGlyph, this]() {
			FT_Face face;
			if (FT_New_Face(LinaVG::Text::g_textData.m_ftlib, item->absolutePath.c_str(), 0, &face))
			{
				LINA_ERR("FileManager: Failed creating new font face for thumbnail!");
				return;
			}

			FT_Error err = FT_Set_Pixel_Sizes(face, 0, RESOURCE_THUMBNAIL_SIZE / 2);

			if (err)
			{
				LINA_ERR("FileManager: Failed setting font pixel sizes for thumbnail!");
				return;
			}

			err = FT_Select_Charmap(face, ft_encoding_unicode);

			if (err)
			{
				LINA_ERR("FileManager: Failed selecting font charmap for thumbnail!");
				return;
			}

			uint32		   glyphW1 = 0;
			uint32		   glyphW2 = 0;
			uint32		   glyphH1 = 0;
			uint32		   glyphH2 = 0;
			unsigned char* buffer  = loadGlyph(face, 65, glyphW1, glyphH1); // A
			unsigned char* buffer2 = loadGlyph(face, 97, glyphW2, glyphH2); // a

			if (buffer != nullptr && buffer2 != nullptr)
			{
				LinaGX::TextureBuffer glyphBuffer1 = {
					.pixels		   = reinterpret_cast<uint8*>(buffer),
					.width		   = glyphW1,
					.height		   = glyphH1,
					.bytesPerPixel = 1,
				};

				LinaGX::TextureBuffer glyphBuffer2 = {
					.pixels		   = reinterpret_cast<uint8*>(buffer2),
					.width		   = glyphW2,
					.height		   = glyphH2,
					.bytesPerPixel = 1,
				};

				LinaGX::TextureBuffer thumbnailBuffer = {
					.pixels		   = new uint8[RESOURCE_THUMBNAIL_SIZE * RESOURCE_THUMBNAIL_SIZE],
					.width		   = RESOURCE_THUMBNAIL_SIZE,
					.height		   = RESOURCE_THUMBNAIL_SIZE,
					.bytesPerPixel = 1,
				};

				MEMSET(thumbnailBuffer.pixels, 0, RESOURCE_THUMBNAIL_SIZE * RESOURCE_THUMBNAIL_SIZE);

				const uint32 widthTotal = glyphW1 + glyphW2;
				const uint32 startX1	= RESOURCE_THUMBNAIL_SIZE / 2 - (widthTotal / 2);
				const uint32 startY1	= RESOURCE_THUMBNAIL_SIZE / 2 - (glyphH1 / 2);
				LinaGX::WriteToBuffer(thumbnailBuffer, glyphBuffer1, startX1, startY1);

				const uint32 startX2 = startX1 + glyphW1 + 1;
				const uint32 startY2 = RESOURCE_THUMBNAIL_SIZE / 2 - (glyphH2 / 2);
				LinaGX::WriteToBuffer(thumbnailBuffer, glyphBuffer2, startX2, startY2);

				item->thumbnail->SetCustomData(thumbnailBuffer.pixels, thumbnailBuffer.width, thumbnailBuffer.height, thumbnailBuffer.bytesPerPixel, LinaGX::ImageChannelMask::G, LinaGX::Format::R8_UNORM);

				delete[] thumbnailBuffer.pixels;
			}

			if (buffer)
				FREE(buffer);

			if (buffer2)
				FREE(buffer2);

			// loadGlyph(face, 97); // a

			FT_Done_Face(face);
		});

		m_editor->GetSystem()->GetMainExecutor()->RunMove(tf);
	}

	void FileManager::GenerateThumbMaterial(DirectoryItem* item)
	{

		Taskflow tf;
		tf.emplace([this]() {
			// EntityWorld world;
			//
			// Entity* camera = world.CreateEntity("Camera");
			// CameraComponent* cam = world.AddComponent<CameraComponent>(camera);
			// camera->SetPosition(Vector3());
			// camera->SetRotation(Quaternion());
			//
			// auto* rm = m_editor->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
			// ResourceIdentifier ident(item->relativePath, GetTypeID<Model>(), TO_SID(item->relativePath));
			// rm->LoadResources({ident});
			// rm->WaitForAll();
			//
			// auto* modelRes = rm->GetResource<Model>();
			// Entity* sphere = world.CreateEntity("Sphere");
			// ModelComponent* model = world.AddComponent<ModelComponent>(sphere);
			// model->SetModel(modelRes);
			//
			// world.GetRenderer()->SetExternalTexture(texture);
			// world.GetRenderer()->Render();
			// gfxManager->Join();
		});
		m_editor->GetSystem()->GetMainExecutor()->RunMove(tf);
	}

	void FileManager::GenerateThumbModel(DirectoryItem* item)
	{
		Taskflow tf;
		tf.emplace([this]() {
			// EntityWorld* world = new EntityWorld();
			// auto* gfxManager = m_editor->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
			// gfxManager->CreateWorldRenderer(world, Vector2ui(RESOURCE_THUMBNAIL_SIZE, RESOURCE_THUMBNAIL_SIZE));
			// LINA_TRACE("HADI BAKALIM");
		});
		m_editor->GetSystem()->GetMainExecutor()->RunMove(tf);
	}

	void FileManager::GenerateThumbShader(DirectoryItem* item)
	{
		Taskflow tf;
		tf.emplace([]() {

		});
		m_editor->GetSystem()->GetMainExecutor()->RunMove(tf);
	}

	void FileManager::FillPathInformation(DirectoryItem* item, const String& fullAbsPath)
	{
		const String basePath = FileSystem::GetFilePath(m_editor->GetProjectData()->GetPath());
		const size_t baseSz	  = basePath.size();

		item->absolutePath = fullAbsPath;
		item->relativePath = fullAbsPath.substr(baseSz, fullAbsPath.size());
		if (item->isDirectory)
			item->folderName = FileSystem::GetLastFolderFromPath(fullAbsPath);
		else
			item->fileName = FileSystem::GetFilenameAndExtensionFromPath(item->absolutePath);

		item->sid = TO_SID(item->relativePath);
	}

	DirectoryItem* FileManager::FindItemFromRelativePath(const String& relativePath, DirectoryItem* searchRoot)
	{
		if (searchRoot->relativePath.compare(relativePath) == 0)
			return searchRoot;

		for (auto* c : searchRoot->children)
		{
			DirectoryItem* item = FindItemFromRelativePath(relativePath, c);
			if (item)
				return item;
		}

		return nullptr;
	}
} // namespace Lina::Editor
