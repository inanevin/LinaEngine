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
#include "Editor/CommonEditor.hpp"

#include "Common/Serialization/Serialization.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/JobSystem/JobSystem.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Audio/Audio.hpp"
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
	LinaGX::TextureBuffer ThumbnailGenerator::GenerateThumbnailForResource(Resource* resource)
	{
		if (resource->GetTID() == GetTypeID<Texture>())
			return GenerateThumbnail(static_cast<Texture*>(resource));

		if (resource->GetTID() == GetTypeID<Model>())
			return GenerateThumbnail(static_cast<Model*>(resource));

		if (resource->GetTID() == GetTypeID<Material>())
			return GenerateThumbnail(static_cast<Material*>(resource));

		if (resource->GetTID() == GetTypeID<Audio>())
			return GenerateThumbnail(static_cast<Audio*>(resource));

		if (resource->GetTID() == GetTypeID<Font>())
			return GenerateThumbnailForFont(static_cast<Font*>(resource)->GetPath());

		return {};
	}

	LinaGX::TextureBuffer ThumbnailGenerator::GenerateThumbnail(Texture* texture)
	{
		LinaGX::TextureBuffer				 buffer	   = {};
		const Vector<LinaGX::TextureBuffer>& allLevels = texture->GetAllLevels();
		buffer										   = allLevels.front();
		// LinaGX::LoadImageFromFile(texture->GetPath().c_str(), buffer, 4, nullptr, true);

		const float max	   = static_cast<float>(Math::Max(buffer.width, buffer.height));
		const float min	   = static_cast<float>(Math::Min(buffer.width, buffer.height));
		const float aspect = max / min;

		uint32 width  = RESOURCE_THUMBNAIL_SIZE;
		uint32 height = RESOURCE_THUMBNAIL_SIZE;
		if (buffer.width > buffer.height)
			height = static_cast<uint32>(static_cast<float>(width) / aspect);
		else
			width = static_cast<uint32>(static_cast<float>(height) / aspect);

		LinaGX::TextureBuffer resizedBuffer = {
			.pixels		   = new uint8[width * height * 4],
			.width		   = width,
			.height		   = height,
			.bytesPerPixel = 4,
		};

		if (!LinaGX::ResizeBuffer(buffer, resizedBuffer, width, height, LinaGX::MipmapFilter::Default, 4, true))
		{
			LINA_ERR("Thumbnail Generator: Failed resizing image for thumbnail!");
		}

		return resizedBuffer;
	}

	LinaGX::TextureBuffer ThumbnailGenerator::GenerateThumbnail(Audio* audio)
	{
		LinaGX::TextureBuffer buffer;
		return buffer;
	}

	LinaGX::TextureBuffer ThumbnailGenerator::GenerateThumbnail(Material* mat)
	{
		LinaGX::TextureBuffer buffer;
		return buffer;
	}

	LinaGX::TextureBuffer ThumbnailGenerator::GenerateThumbnail(Model* model)
	{
		/*
		// Snapshot buffer
		Buffer buffer;
		buffer.Create(LinaGX::ResourceTypeHint::TH_ReadbackDest, RESOURCE_THUMBNAIL_SIZE * RESOURCE_THUMBNAIL_SIZE * 4, "Snapshot", true);

		// Create world.
		const Vector2ui viewSize = Vector2ui(RESOURCE_THUMBNAIL_SIZE, RESOURCE_THUMBNAIL_SIZE);
		EntityWorld*	world	 = new EntityWorld();
		world->SetRenderSize(viewSize);
		WorldRenderer* renderer = new WorldRenderer(world, viewSize, &buffer, true);

		// const String projectPath = FileSystem::GetFilePath(m_editor->GetProjectManager().GetProjectData()->GetPath());
		// const String cachePath     = projectPath + "/ResourceCache/";

		Vector<ResourceIdentifier> resources;
		// resources.push_back({"Resources/Core/Models/SkyCube.glb", GetTypeID<Model>()});
		// resources.push_back({item->relativePath, GetTypeID<Model>()});
		// resources.push_back({"Resources/Core/Shaders/Object/DeferredLighting.linashader", GetTypeID<Shader>()});
		// resources.push_back({"Resources/Core/Shaders/Object/DefaultObject.linashader", GetTypeID<Shader>()});
		// resources.push_back({"Resources/Core/Shaders/Sky/DefaultSky.linashader", GetTypeID<Shader>()});
		// world->GetResourceManagerV2().LoadResourcesFromFile(m_editor, 0, resources, cachePath, projectPath);
		// world->GetResourceManagerV2().WaitForAll();

		// Model*      model               = world->GetResourceManagerV2().GetResource<Model>(TO_SID(item->relativePath));
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
		Application::GetLGX()->WaitForUserSemaphore(semaphoreData.GetSemaphore(), semaphoreData.GetValue());

		LinaGX::TextureBuffer retBuffer = {
			.width		   = RESOURCE_THUMBNAIL_SIZE,
			.height		   = RESOURCE_THUMBNAIL_SIZE,
			.bytesPerPixel = 4,
			.pixels		   = new uint8[RESOURCE_THUMBNAIL_SIZE * RESOURCE_THUMBNAIL_SIZE * 4],
		};
		MEMCPY(retBuffer.pixels, buffer.GetMapped(), RESOURCE_THUMBNAIL_SIZE * RESOURCE_THUMBNAIL_SIZE * 4);
		buffer.Destroy();
		delete renderer;
		delete world;
		return retBuffer;
		 */
		return {};
	}

	LinaGX::TextureBuffer ThumbnailGenerator::GenerateThumbnailForFont(const String& absPath)
	{
		LinaGX::TextureBuffer thumbnailBuffer;

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
		if (FT_New_Face(LinaVG::g_ftLib, absPath.c_str(), 0, &face))
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

			thumbnailBuffer = {
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
		}

		if (buffer)
			FREE(buffer);

		if (buffer2)
			FREE(buffer2);

		FT_Done_Face(face);

		return thumbnailBuffer;
	}

} // namespace Lina::Editor
