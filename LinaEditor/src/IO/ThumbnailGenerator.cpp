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

#include "Editor/Editor.hpp"
#include "Editor/IO/ThumbnailGenerator.hpp"
#include "Editor/IO/DirectoryItem.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/Popups/NotificationDisplayer.hpp"
#include "Common/System/System.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Audio/Audio.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Data/Mesh.hpp"
#include "Core/Graphics/Resource/Mesh.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Components/MeshComponent.hpp"
#include "Core/Components/CameraComponent.hpp"
#include "Core/World/EntityWorld.hpp"

namespace Lina::Editor
{

	void ThumbnailGenerator::Initialize(Editor* editor)
	{
		m_editor = editor;
	}

	void ThumbnailGenerator::KickOffBatch(RequestBatch* batch)
	{
		NotificationDesc notification = {
			.icon		= NotificationIcon::Loading,
			.title		= Locale::GetStr(LocaleStr::GeneratingThumbnails),
			.showButton = false,
			.onProgress = [this, batch](float& out) { out = static_cast<float>(batch->generatedCount.load()) / static_cast<float>(batch->totalCount); },
		};
		m_editor->GetWindowPanelManager().GetNotificationDisplayer(m_editor->GetWindowPanelManager().GetMainWindow())->AddNotification(notification);

		m_editor->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager)->Lock();

		Taskflow tf;
		tf.emplace([batch, this]() {
			for (DirectoryItem* item : batch->requests)
			{
				GenerateThumbnailForItem(item, batch);
				batch->generatedCount.fetch_add(1);
			}
		});

		m_executor.RunMove(tf, [batch, this]() {
			for (auto& p : batch->atlases)
			{
				p.first->textureAtlas = p.second;
			}

			delete batch;
			m_editor->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager)->Unlock();
			m_editor->GetAtlasManager().RefreshDirtyAtlases();
		});
	}

	void ThumbnailGenerator::PreTick()
	{
		if (!m_thumbnailRequests.empty())
		{
			for (DirectoryItem* item : m_thumbnailRequests)
			{
				if (item->textureAtlas != nullptr)
					m_editor->GetAtlasManager().RemoveImage(item->textureAtlas);
				item->textureAtlas = nullptr;
			}

			RequestBatch* batch = new RequestBatch{
				.totalCount		= static_cast<uint32>(m_thumbnailRequests.size()),
				.generatedCount = 0,
				.requests		= m_thumbnailRequests,
			};
			m_thumbnailRequests.clear();
			KickOffBatch(batch);
		}
	}

	void ThumbnailGenerator::Shutdown()
	{
	}

	void ThumbnailGenerator::GenerateThumbnail(DirectoryItem* item, bool isRecursive)
	{
		const String cachePath = FileSystem::GetUserDataFolder() + "Editor/Thumbnails/";
		if (!FileSystem::FileOrPathExists(cachePath))
			FileSystem::CreateFolderInPath(cachePath);

		if (!item->isDirectory)
			m_thumbnailRequests.push_back(item);

		if (isRecursive)
		{
			for (DirectoryItem* child : item->children)
				GenerateThumbnail(child, isRecursive);
		}
	}

	void ThumbnailGenerator::GenerateThumbnailForItem(DirectoryItem* item, RequestBatch* batch)
	{
		const String thumbnailPath = FileSystem::GetUserDataFolder() + "Editor/Thumbnails/ResourceThumbnail_" + TO_STRING(item->sid);

		if (item->tid == GetTypeID<Texture>())
		{
			GenerateThumbTexture(item, thumbnailPath, batch);
		}
		else if (item->tid == GetTypeID<Font>())
		{
			GenerateThumbFont(item, thumbnailPath, batch);
		}
		else if (item->tid == GetTypeID<Model>())
		{
			// GenerateThumbModel(item, thumbnailPath);
		}
		else if (item->tid == GetTypeID<Material>())
		{
			// GenerateThumbMaterial(item, thumbnailPath);
		}
		else if (item->tid == GetTypeID<Shader>())
		{
			// set to shader icon.
		}
		else if (item->tid == GetTypeID<Audio>())
		{
			// set to audio icon.
		}

		return;

		if (FileSystem::FileOrPathExists(thumbnailPath))
		{
			// IStream input    = Serialization::LoadFromFile(thumbnailFullPath.c_str());
			// uint32 bytesPerPixel = 0;
			// Vector2ui size = Vector2ui::Zero;
			// input >> size.x >> size.y >> bytesPerPixel;
			// item->textureAtlas = m_editor->GetAtlasManager().AddImageToAtlas(input.GetDataCurrent(), size, bytesPerPixel);
			// input.Destroy();
			// return;
		}
	}

	void ThumbnailGenerator::GenerateThumbTexture(DirectoryItem* item, const String& thumbPath, RequestBatch* batch)
	{
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
				OStream stream;
				stream << resizedBuffer.width << resizedBuffer.height << resizedBuffer.bytesPerPixel;
				stream.WriteRaw(resizedBuffer.pixels, resizedBuffer.width * resizedBuffer.height * resizedBuffer.bytesPerPixel);
				Serialization::SaveToFile(thumbPath.c_str(), stream);

				TextureAtlasImage* atlas = m_editor->GetAtlasManager().AddImageToAtlas(resizedBuffer.pixels, Vector2ui(resizedBuffer.width, resizedBuffer.height), resizedBuffer.bytesPerPixel);
				batch->atlases.try_emplace(item, atlas);
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
	}

	void ThumbnailGenerator::GenerateThumbFont(DirectoryItem* item, const String& thumbPath, RequestBatch* batch)
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

		FT_Face face;
		if (FT_New_Face(LinaVG::g_ftLib, item->absolutePath.c_str(), 0, &face))
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

			OStream stream;
			stream << thumbnailBuffer.width << thumbnailBuffer.height << thumbnailBuffer.bytesPerPixel;
			stream.WriteRaw(thumbnailBuffer.pixels, thumbnailBuffer.width * thumbnailBuffer.height * 1);
			Serialization::SaveToFile(thumbPath.c_str(), stream);

			TextureAtlasImage* atlas = m_editor->GetAtlasManager().AddImageToAtlas(thumbnailBuffer.pixels, Vector2ui(thumbnailBuffer.width, thumbnailBuffer.height), thumbnailBuffer.bytesPerPixel);
			batch->atlases.try_emplace(item, atlas);
			delete[] thumbnailBuffer.pixels;
		}

		if (buffer)
			FREE(buffer);

		if (buffer2)
			FREE(buffer2);

		// loadGlyph(face, 97); // a

		FT_Done_Face(face);
	}

	void ThumbnailGenerator::GenerateThumbMaterial(DirectoryItem* item, const String& thumbPath, RequestBatch* batch)
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

	void ThumbnailGenerator::GenerateThumbModel(DirectoryItem* item, const String& thumbPath, RequestBatch* batch)
	{
		auto* rm  = m_editor->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		auto* gfx = m_editor->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
		auto* wm  = m_editor->GetSystem()->CastSubsystem<WorldManager>(SubsystemType::WorldManager);

		// Load the model.
		ResourceIdentifier ident;
		ident.absolutePath = item->absolutePath;
		ident.relativePath = item->relativePath;
		ident.tid		   = item->tid;
		rm->LoadResourcesFromFile(0, {ident}, "");
		rm->WaitForAll();

		// Create world.
		EntityWorld* world = new EntityWorld(m_editor->GetSystem());
		world->SetSkyMaterial(rm->GetResource<Material>(DEFAULT_MATERIAL_SKY_SID));
		world->SetRenderSize(Vector2ui(RESOURCE_THUMBNAIL_SIZE, RESOURCE_THUMBNAIL_SIZE));

		// Setup world
		Model*		model		 = rm->GetResource<Model>(TO_SID(ident.relativePath));
		const AABB& aabb		 = model->GetAABB();
		Entity*		cameraEntity = world->CreateEntity("Camera");
		cameraEntity->SetPosition(Vector3(0, aabb.boundsHalfExtents.y * 2, -aabb.boundsHalfExtents.z * 4));
		cameraEntity->SetRotation(Quaternion::LookAt(cameraEntity->GetPosition(), Vector3::Zero, Vector3::Up));
		CameraComponent* camera = world->AddComponent<CameraComponent>(cameraEntity);
		world->SetActiveCamera(camera);

		const auto& meshes = model->GetMeshes();
		uint32		idx	   = 0;
		for (auto* mesh : meshes)
		{
			Entity*		   entity = world->CreateEntity(mesh->GetName());
			MeshComponent* m	  = world->AddComponent<MeshComponent>(entity);
			m->SetMesh(TO_SID(ident.relativePath), idx);
			m->SetMaterial(DEFAULT_MATERIAL_OBJECT_SID);
			idx++;
		}

		Buffer buffer;
		buffer.Create(gfx->GetLGX(), LinaGX::ResourceTypeHint::TH_ReadbackDest, RESOURCE_THUMBNAIL_SIZE * RESOURCE_THUMBNAIL_SIZE * 4, "Snapshot", true);
		world->PreTick();
		world->Tick(0.016f);

		// Render.
		WorldRenderer* wr = new WorldRenderer(gfx, world, world->GetRenderSize(), &buffer);
		gfx->AddRenderer(wr, "WorldRenderers"_hs);
		gfx->Join();
		gfx->Render("WorldRenderers"_hs);
		gfx->Join();
		gfx->RemoveRenderer(wr);
		delete wr;
		delete world;

		item->textureAtlas = m_editor->GetAtlasManager().AddImageToAtlas(buffer.GetMapped(), Vector2ui(RESOURCE_THUMBNAIL_SIZE, RESOURCE_THUMBNAIL_SIZE), 4);

		OStream stream;
		stream << RESOURCE_THUMBNAIL_SIZE << RESOURCE_THUMBNAIL_SIZE << 4;
		stream.WriteRaw(buffer.GetMapped(), RESOURCE_THUMBNAIL_SIZE * RESOURCE_THUMBNAIL_SIZE * 4);
		Serialization::SaveToFile(thumbPath.c_str(), stream);

		buffer.Destroy();
	}

} // namespace Lina::Editor
