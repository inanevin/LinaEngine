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
#include "Editor/CommonEditor.hpp"
#include "Editor/Widgets/Popups/NotificationDisplayer.hpp"
#include "Common/System/System.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/JobSystem/JobSystem.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Audio/Audio.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Data/Mesh.hpp"
#include "Core/Graphics/Resource/Mesh.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Components/MeshComponent.hpp"
#include "Core/Components/CameraComponent.hpp"
#include "Core/World/EntityWorld.hpp"

namespace Lina::Editor
{

	ThumbnailGenerator::ThumbnailGenerator(Editor* editor, JobExecutor* executor, DirectoryItem* item, bool isRecursive)
	{
		m_editor	 = editor;
		m_executor	 = executor;
		m_rm		 = &editor->GetResourceManagerV2();
		m_gfxManager = m_editor->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);

		const String cachePath = FileSystem::GetUserDataFolder() + "Editor/Thumbnails/";
		if (!FileSystem::FileOrPathExists(cachePath))
			FileSystem::CreateFolderInPath(cachePath);

		CollectItems(item, isRecursive);

		m_totalCount = static_cast<uint32>(m_thumbnailItems.size());
		// Start notification
		NotificationDesc notification = {
			.icon		= NotificationIcon::Loading,
			.title		= Locale::GetStr(LocaleStr::GeneratingThumbnails),
			.showButton = false,
			.onProgress = [this](float& out) { out = static_cast<float>(m_generatedCount.load()) / static_cast<float>(m_totalCount); },
		};
		m_editor->GetWindowPanelManager().GetNotificationDisplayer(m_editor->GetWindowPanelManager().GetMainWindow())->AddNotification(notification);

		Taskflow tf;

		for (DirectoryItem* item : m_thumbnailItems)
		{
			tf.emplace([this, item]() {
				const String thumbnailPath = FileSystem::GetUserDataFolder() + "Editor/Thumbnails/ResourceThumbnail_" + TO_STRING(item->sid);

				if (FileSystem::FileOrPathExists(thumbnailPath))
				{
					IStream	 input			  = Serialization::LoadFromFile(thumbnailPath.c_str());
					StringID lastModifiedDate = 0;
					input >> lastModifiedDate;

					if (item->lastModifiedDate != lastModifiedDate)
					{
						input.Destroy();
						FileSystem::DeleteFileInPath(thumbnailPath);
					}
					else
					{
						uint32	  bytesPerPixel = 0;
						Vector2ui size			= Vector2ui::Zero;
						input >> size.x >> size.y >> bytesPerPixel;
						TextureAtlasImage* atlas = m_editor->GetAtlasManager().AddImageToAtlas(input.GetDataCurrent(), size, bytesPerPixel);
						m_atlases.try_emplace(item, atlas);
						input.Destroy();
						m_generatedCount.fetch_add(1);
						return;
					}
				}

				if (item->tid == GetTypeID<Texture>())
				{
					GenerateThumbTexture(item, thumbnailPath);
					m_generatedCount.fetch_add(1);
				}
				else if (item->tid == GetTypeID<Font>())
				{
					GenerateThumbFont(item, thumbnailPath);
					m_generatedCount.fetch_add(1);
				}
				else if (item->tid == GetTypeID<Model>())
				{
					GenerateThumbModel(item, thumbnailPath);
					m_generatedCount.fetch_add(1);
				}
				else if (item->tid == GetTypeID<Material>())
				{
					GenerateThumbMaterial(item, thumbnailPath);
					m_generatedCount.fetch_add(1);
				}
				else if (item->tid == GetTypeID<Shader>())
				{
					TextureAtlasImage* img = m_editor->GetAtlasManager().GetImageFromAtlas("ProjectIcons"_hs, "FileShader"_hs);
					m_atlases.try_emplace(item, img);
					m_generatedCount.fetch_add(1);
				}
				else if (item->tid == GetTypeID<Audio>())
				{
					TextureAtlasImage* img = m_editor->GetAtlasManager().GetImageFromAtlas("ProjectIcons"_hs, "FileAudio"_hs);
					m_atlases.try_emplace(item, img);
					m_generatedCount.fetch_add(1);
				}
				else
					m_generatedCount.fetch_add(1);
			});
		}

		m_future = m_executor->RunMove(tf, [this]() {
			for (const Pair<DirectoryItem*, TextureAtlasImage*>& p : m_atlases)
				p.first->textureAtlas = p.second;
			m_editor->GetAtlasManager().RefreshDirtyAtlases();
			m_status.store(Status::Done);
		});
	}

	ThumbnailGenerator::~ThumbnailGenerator()
	{
		if (m_future.valid())
		{
			m_future.cancel();
			m_future.get();
		}
	}

	void ThumbnailGenerator::CollectItems(DirectoryItem* item, bool isRecursive)
	{
		if (!item->isDirectory)
			m_thumbnailItems.push_back(item);

		if (isRecursive)
		{
			for (DirectoryItem* child : item->children)
				CollectItems(child, isRecursive);
		}
	}

	void ThumbnailGenerator::GenerateThumbTexture(DirectoryItem* item, const String& thumbPath)
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
				stream << item->lastModifiedDate;
				stream << resizedBuffer.width << resizedBuffer.height << resizedBuffer.bytesPerPixel;
				stream.WriteRaw(resizedBuffer.pixels, resizedBuffer.width * resizedBuffer.height * resizedBuffer.bytesPerPixel);
				Serialization::SaveToFile(thumbPath.c_str(), stream);
				stream.Destroy();

				TextureAtlasImage* atlas = m_editor->GetAtlasManager().AddImageToAtlas(resizedBuffer.pixels, Vector2ui(resizedBuffer.width, resizedBuffer.height), resizedBuffer.bytesPerPixel);
				m_atlases.try_emplace(item, atlas);
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

	void ThumbnailGenerator::GenerateThumbFont(DirectoryItem* item, const String& thumbPath)
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
			stream << item->lastModifiedDate;
			stream << thumbnailBuffer.width << thumbnailBuffer.height << thumbnailBuffer.bytesPerPixel;
			stream.WriteRaw(thumbnailBuffer.pixels, thumbnailBuffer.width * thumbnailBuffer.height * 1);
			Serialization::SaveToFile(thumbPath.c_str(), stream);
			stream.Destroy();

			TextureAtlasImage* atlas = m_editor->GetAtlasManager().AddImageToAtlas(thumbnailBuffer.pixels, Vector2ui(thumbnailBuffer.width, thumbnailBuffer.height), thumbnailBuffer.bytesPerPixel);
			m_atlases.try_emplace(item, atlas);
			delete[] thumbnailBuffer.pixels;
		}

		if (buffer)
			FREE(buffer);

		if (buffer2)
			FREE(buffer2);

		FT_Done_Face(face);
	}

	void ThumbnailGenerator::GenerateThumbMaterial(DirectoryItem* item, const String& thumbPath)
	{
		// Snapshot buffer
		Buffer buffer;
		buffer.Create(LinaGX::ResourceTypeHint::TH_ReadbackDest, RESOURCE_THUMBNAIL_SIZE * RESOURCE_THUMBNAIL_SIZE * 4, "Snapshot", true);

		// Create world.
		const Vector2ui viewSize = Vector2ui(RESOURCE_THUMBNAIL_SIZE, RESOURCE_THUMBNAIL_SIZE);
		EntityWorld*	world	 = new EntityWorld();
		world->SetRenderSize(viewSize);
		WorldRenderer* renderer = new WorldRenderer(world, viewSize, &buffer, true);

		String metacachePath = "";
		FileManager::GetMetacachePath(metacachePath);
		const String projectPath = FileSystem::GetFilePath(m_editor->GetProjectManager().GetProjectData()->GetPath());

		Vector<ResourceIdentifier> resources;
		resources.push_back({"Resources/Core/Models/SkyCube.glb", GetTypeID<Model>()});
		resources.push_back({"Resources/Core/Models/Sphere.glb", GetTypeID<Model>()});
		resources.push_back({"Resources/Core/Shaders/Object/DeferredLighting.linashader", GetTypeID<Shader>()});
		resources.push_back({"Resources/Core/Shaders/Object/DefaultObject.linashader", GetTypeID<Shader>()});
		resources.push_back({"Resources/Core/Shaders/Sky/DefaultSky.linashader", GetTypeID<Shader>()});
		resources.push_back({item->relativePath, GetTypeID<Material>()});
		world->GetResourceManagerV2().LoadResourcesFromFile(m_editor, 0, resources, metacachePath, projectPath);
		world->GetResourceManagerV2().WaitForAll();

		Material* objectMaterial = world->GetResourceManagerV2().GetResource<Material>(TO_SID(item->relativePath));
		resources.clear();
		resources.push_back({objectMaterial->GetShaderPath(), GetTypeID<Shader>()});
		world->GetResourceManagerV2().LoadResourcesFromFile(m_editor, 0, resources, metacachePath, projectPath);
		world->GetResourceManagerV2().WaitForAll();

		Model*	  model			   = world->GetResourceManagerV2().GetResource<Model>("Resources/Core/Models/Sphere.glb"_hs);
		Material* skyMaterial	   = world->GetResourceManagerV2().CreateResource<Material>("SkyMaterial", "SkyMaterial"_hs);
		Material* lightingMaterial = world->GetResourceManagerV2().CreateResource<Material>("LightingMaterial", "LightingMaterial"_hs);
		world->GetGfxSettings().SetSkyMaterial(skyMaterial);
		world->GetGfxSettings().SetLightingMaterial(lightingMaterial);

		skyMaterial->SetShader(world->GetResourceManagerV2().GetResource<Shader>("Resources/Core/Shaders/Sky/DefaultSky.linashader"_hs));
		lightingMaterial->SetShader(world->GetResourceManagerV2().GetResource<Shader>("Resources/Core/Shaders/Object/DeferredLighting.linashader"_hs));
		objectMaterial->SetShader(world->GetResourceManagerV2().GetResource<Shader>("Resources/Core/Shaders/Object/DefaultObject.linashader"_hs));

		renderer->MarkBindlessDirty();

		// Setup world
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
			m->SetMesh(model, idx);
			m->SetMaterial(objectMaterial);
			idx++;
		}

		world->PreTick();
		world->Tick(0.016f);
		renderer->Tick(0.016f);
		renderer->Render(0);

		const SemaphoreData semaphoreData = renderer->GetSubmitSemaphore(0);
		GfxManager::GetLGX()->WaitForUserSemaphore(semaphoreData.GetSemaphore(), semaphoreData.GetValue());

		TextureAtlasImage* atlas = m_editor->GetAtlasManager().AddImageToAtlas(buffer.GetMapped(), viewSize, 4);
		m_atlases.try_emplace(item, atlas);

		OStream stream;
		stream << item->lastModifiedDate;
		stream << RESOURCE_THUMBNAIL_SIZE << RESOURCE_THUMBNAIL_SIZE << 4;
		stream.WriteRaw(buffer.GetMapped(), RESOURCE_THUMBNAIL_SIZE * RESOURCE_THUMBNAIL_SIZE * 4);
		Serialization::SaveToFile(thumbPath.c_str(), stream);
		stream.Destroy();

		buffer.Destroy();
		delete renderer;
		delete world;
	}

	void ThumbnailGenerator::GenerateThumbModel(DirectoryItem* item, const String& thumbPath)
	{
		// Snapshot buffer
		Buffer buffer;
		buffer.Create(LinaGX::ResourceTypeHint::TH_ReadbackDest, RESOURCE_THUMBNAIL_SIZE * RESOURCE_THUMBNAIL_SIZE * 4, "Snapshot", true);

		// Create world.
		const Vector2ui viewSize = Vector2ui(RESOURCE_THUMBNAIL_SIZE, RESOURCE_THUMBNAIL_SIZE);
		EntityWorld*	world	 = new EntityWorld();
		world->SetRenderSize(viewSize);
		WorldRenderer* renderer = new WorldRenderer(world, viewSize, &buffer, true);

		String metacachePath = "";
		FileManager::GetMetacachePath(metacachePath);
		const String projectPath = FileSystem::GetFilePath(m_editor->GetProjectManager().GetProjectData()->GetPath());

		Vector<ResourceIdentifier> resources;
		resources.push_back({"Resources/Core/Models/SkyCube.glb", GetTypeID<Model>()});
		resources.push_back({item->relativePath, GetTypeID<Model>()});
		resources.push_back({"Resources/Core/Shaders/Object/DeferredLighting.linashader", GetTypeID<Shader>()});
		resources.push_back({"Resources/Core/Shaders/Object/DefaultObject.linashader", GetTypeID<Shader>()});
		resources.push_back({"Resources/Core/Shaders/Sky/DefaultSky.linashader", GetTypeID<Shader>()});
		world->GetResourceManagerV2().LoadResourcesFromFile(m_editor, 0, resources, metacachePath, projectPath);
		world->GetResourceManagerV2().WaitForAll();

		Model*	  model			   = world->GetResourceManagerV2().GetResource<Model>(TO_SID(item->relativePath));
		Material* skyMaterial	   = world->GetResourceManagerV2().CreateResource<Material>("SkyMaterial", "SkyMaterial"_hs);
		Material* lightingMaterial = world->GetResourceManagerV2().CreateResource<Material>("LightingMaterial", "LightingMaterial"_hs);
		Material* objectMaterial   = world->GetResourceManagerV2().CreateResource<Material>("ObjectMaterial", "ObjectMaterial"_hs);

		world->GetGfxSettings().SetSkyMaterial(skyMaterial);
		world->GetGfxSettings().SetLightingMaterial(lightingMaterial);

		skyMaterial->SetShader(world->GetResourceManagerV2().GetResource<Shader>("Resources/Core/Shaders/Sky/DefaultSky.linashader"_hs));
		lightingMaterial->SetShader(world->GetResourceManagerV2().GetResource<Shader>("Resources/Core/Shaders/Object/DeferredLighting.linashader"_hs));
		objectMaterial->SetShader(world->GetResourceManagerV2().GetResource<Shader>("Resources/Core/Shaders/Object/DefaultObject.linashader"_hs));

		renderer->MarkBindlessDirty();

		// Setup world
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
			m->SetMesh(model, idx);
			m->SetMaterial(objectMaterial);
			idx++;
		}

		world->PreTick();
		world->Tick(0.016f);
		renderer->Tick(0.016f);
		renderer->Render(0);

		const SemaphoreData semaphoreData = renderer->GetSubmitSemaphore(0);
		GfxManager::GetLGX()->WaitForUserSemaphore(semaphoreData.GetSemaphore(), semaphoreData.GetValue());

		TextureAtlasImage* atlas = m_editor->GetAtlasManager().AddImageToAtlas(buffer.GetMapped(), viewSize, 4);
		m_atlases.try_emplace(item, atlas);

		OStream stream;
		stream << item->lastModifiedDate;
		stream << RESOURCE_THUMBNAIL_SIZE << RESOURCE_THUMBNAIL_SIZE << 4;
		stream.WriteRaw(buffer.GetMapped(), RESOURCE_THUMBNAIL_SIZE * RESOURCE_THUMBNAIL_SIZE * 4);
		Serialization::SaveToFile(thumbPath.c_str(), stream);
		stream.Destroy();

		buffer.Destroy();
		delete renderer;
		delete world;
	}

} // namespace Lina::Editor
