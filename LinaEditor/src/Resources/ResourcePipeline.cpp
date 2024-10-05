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

#include "Editor/Resources/ResourcePipeline.hpp"
#include "Editor/Editor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/IO/ThumbnailGenerator.hpp"
#include "Editor/Widgets/Panel/PanelResourceBrowser.hpp"
#include "Editor/Widgets/Compound/ResourceDirectoryBrowser.hpp"
#include "Editor/Widgets/Popups/ProgressPopup.hpp"

#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Audio/Audio.hpp"
#include "Core/Physics/PhysicsMaterial.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Editor/Widgets/Popups/NotificationDisplayer.hpp"

namespace Lina::Editor
{

	ResourceDirectory* ResourcePipeline::SaveNewResource(ProjectData* project, ResourceDirectory* src, const String& name, TypeID tid, ResourceID inID, uint32 subType)
	{
		const ResourceID id	  = inID != 0 ? inID : project->ConsumeResourceID();
		const String	 path = project->GetResourcePath(id);

		ResourceDirectory* newCreated = src->CreateChild({
			.isFolder	 = false,
			.resourceID	 = id,
			.resourceTID = tid,
			.name		 = name,
		});

		if (tid == GetTypeID<GUIWidget>())
		{
			GUIWidget w(id, name);
			w.GetRoot().GetWidgetProps().debugName = "Root";
			w.SaveToFileAsBinary(path);
		}
		else if (tid == GetTypeID<Material>())
		{
			Material mat(id, name);
			mat.SaveToFileAsBinary(path);
		}
		else if (tid == GetTypeID<EntityWorld>())
		{
			EntityWorld world(id, name);
			world.SaveToFileAsBinary(path);
		}
		else if (tid == GetTypeID<TextureSampler>())
		{
			TextureSampler sampler(id, name);
			sampler.SaveToFileAsBinary(path);
		}
		else if (tid == GetTypeID<Shader>())
		{
			Shader shader(id, name);

			// Load default text.
			if (subType == 0)
			{
			}
			else if (subType == 1)
			{
			}
			else if (subType == 2)
			{
			}
			else if (subType == 3)
			{
			}
			shader.SaveToFileAsBinary(path);
		}
		else if (tid == GetTypeID<PhysicsMaterial>())
		{
			PhysicsMaterial mat(id, name);
			mat.SaveToFileAsBinary(path);
		}

		return newCreated;
	}

	void ResourcePipeline::ImportResources(ProjectData* projectData, ResourceDirectory* src, const Vector<ResourceImportDef>& defs, Delegate<void(uint32 imported, const ResourceImportDef& importDef, bool isCompleted)> onProgress)
	{
		Shader* defaultShader = nullptr;

		const int32 sz = static_cast<int32>(defs.size());

		for (int32 i = 0; i < sz; i++)
		{
			const ResourceImportDef& def = defs.at(i);

			onProgress(i, def, false);

			if (!FileSystem::FileOrPathExists(def.path))
				continue;

			const String   extension = FileSystem::GetFileExtension(def.path);
			const StringID extSid	 = TO_SID(extension);

			TypeID resourceTID = 0;

			if (extSid == "png"_hs || extSid == "jpg"_hs || extSid == "jpeg"_hs)
				resourceTID = GetTypeID<Texture>();
			else if (extSid == "mp3"_hs)
				resourceTID = GetTypeID<Audio>();
			else if (extSid == "glb"_hs || extSid == "gtlf"_hs)
				resourceTID = GetTypeID<Model>();
			else if (extSid == "otf"_hs || extSid == "ttf"_hs)
				resourceTID = GetTypeID<Font>();
			else if (extSid == "linashader"_hs)
				resourceTID = GetTypeID<Shader>();
			else
				continue;

			const String	   name = FileSystem::GetFilenameOnlyFromPath(def.path) + "." + FileSystem::GetFileExtension(def.path);
			ResourceDirectory* dir	= nullptr;

			auto createDirectory = [&](ResourceID id) {
				dir = src->CreateChild({
					.isFolder	 = false,
					.resourceID	 = id,
					.resourceTID = resourceTID,
					.name		 = FileSystem::GetFilenameOnlyFromPath(def.path) + "." + FileSystem::GetFileExtension(def.path),
				});
			};

			auto genThumbnail = [](Resource* r, ResourceDirectory* directory) {
				LinaGX::TextureBuffer thumbnail = ThumbnailGenerator::GenerateThumbnailForResource(r);

				if (thumbnail.pixels != nullptr)
				{
					OStream stream;
					stream << thumbnail.width << thumbnail.height << thumbnail.bytesPerPixel;
					stream.WriteRaw(thumbnail.pixels, thumbnail.width * thumbnail.height * thumbnail.bytesPerPixel);
					directory->thumbnailBuffer.Create(stream);
					stream.Destroy();
				}
			};

			if (resourceTID == GetTypeID<Shader>())
			{
				Shader shader(0, name);
				if (!shader.LoadFromFile(def.path))
					continue;

				shader.SetID(def.id == 0 ? projectData->ConsumeResourceID() : def.id);
				shader.SetPath(def.path);
				shader.SaveToFileAsBinary(projectData->GetResourcePath(shader.GetID()));
				createDirectory(shader.GetID());
				genThumbnail(&shader, dir);
			}
			else if (resourceTID == GetTypeID<Texture>())
			{
				Texture txt(0, name);
				if (!txt.LoadFromFile(def.path))
					continue;

				txt.SetID(def.id == 0 ? projectData->ConsumeResourceID() : def.id);
				txt.SetPath(def.path);
				txt.SaveToFileAsBinary(projectData->GetResourcePath(txt.GetID()));
				createDirectory(txt.GetID());
				genThumbnail(&txt, dir);
			}
			else if (resourceTID == GetTypeID<Font>())
			{
				Font font(0, name);
				if (!font.LoadFromFile(def.path))
					continue;

				font.SetID(def.id == 0 ? projectData->ConsumeResourceID() : def.id);
				font.SetPath(def.path);
				font.SaveToFileAsBinary(projectData->GetResourcePath(font.GetID()));
				createDirectory(font.GetID());
				genThumbnail(&font, dir);
			}
			else if (resourceTID == GetTypeID<Audio>())
			{
				Audio aud(0, name);
				if (aud.LoadFromFile(def.path))
					continue;

				aud.SetID(def.id == 0 ? projectData->ConsumeResourceID() : def.id);
				aud.SetPath(def.path);
				aud.SaveToFileAsBinary(projectData->GetResourcePath(aud.GetID()));
				createDirectory(aud.GetID());
				genThumbnail(&aud, dir);
			}
			else if (resourceTID == GetTypeID<Model>())
			{
				Model model(0, name);
				if (!model.LoadFromFile(def.path))
					continue;

				model.SetID(def.id == 0 ? projectData->ConsumeResourceID() : def.id);
				model.SetPath(def.path);
				createDirectory(model.GetID());
				genThumbnail(&model, dir);

				if (defaultShader == nullptr)
				{
					defaultShader = new Shader(EDITOR_SHADER_DEFAULT_OBJECT_ID, EDITOR_SHADER_DEFAULT_OBJECT_PATH);
					defaultShader->LoadFromFile(EDITOR_SHADER_DEFAULT_OBJECT_PATH);
				}

				const Vector<ModelMaterial>&	   matDefs	   = model.GetMaterialDefs();
				const Vector<Model::ModelTexture>& textureDefs = model.GetTextureDefs();

				Vector<ResourceID> createdTextures;
				Vector<String>	   createdTextureNames;

				for (const Model::ModelTexture& def : textureDefs)
				{
					const ResourceID newID = projectData->ConsumeResourceID();

					ResourceDirectory* child = dir->parent->CreateChild({
						.isFolder	 = false,
						.resourceID	 = newID,
						.resourceTID = GetTypeID<Texture>(),
						.name		 = def.name,
					});

					const bool isColor = true;

					Texture texture(newID, def.name);
					texture.GetMeta().format = isColor ? LinaGX::Format::R8G8B8A8_SRGB : LinaGX::Format::R8G8B8A8_UNORM;
					texture.LoadFromBuffer(def.buffer.pixels, def.buffer.width, def.buffer.height, def.buffer.bytesPerPixel);
					texture.SaveToFileAsBinary(projectData->GetResourcePath(newID));
					genThumbnail(&texture, child);

					createdTextures.push_back(newID);
					createdTextureNames.push_back(def.name);
				}

				Model::Metadata& meta = model.GetMeta();

				int32 matIdx = 0;
				for (const ModelMaterial& def : matDefs)
				{
					const ResourceID newID = projectData->ConsumeResourceID();

					ResourceDirectory* child = dir->parent->CreateChild({
						.isFolder	 = false,
						.resourceID	 = newID,
						.resourceTID = GetTypeID<Material>(),
						.name		 = def.name,
					});

					meta.materials[matIdx] = newID;

					LinaTexture2D albedo = {
						.texture = 0,
						.sampler = 0,
					};

					LinaTexture2D normal = {
						.texture = 0,
						.sampler = 0,
					};

					int32 outIndex	= -1;
					int32 outIndex2 = -1;
					for (auto [textureType, textureIndex] : def.textureIndices)
					{
						if (textureType == static_cast<uint8>(LinaGX::GLTFTextureType::BaseColor))
						{
							albedo.texture = createdTextures[textureIndex];
							outIndex	   = textureIndex;
							continue;
						}

						if (textureType == static_cast<uint8>(LinaGX::GLTFTextureType::Normal))
						{
							normal.texture = createdTextures[textureIndex];
							outIndex2	   = textureIndex;
							continue;
						}
					}

					Material mat(newID, def.name);
					mat.SetShader(defaultShader);
					mat.SetProperty("color"_hs, def.albedo);
					mat.SetProperty("txtAlbedo"_hs, albedo);
					mat.SetProperty("txtNormal"_hs, normal);
					mat.SaveToFileAsBinary(projectData->GetResourcePath(newID));
					genThumbnail(&mat, child);
					matIdx++;
				}
				model.SaveToFileAsBinary(projectData->GetResourcePath(model.GetID()));
				model.DestroyTextureDefs();
			}
		}

		if (defaultShader)
			delete defaultShader;

		onProgress(sz, {}, true);
	}

	void ResourcePipeline::DuplicateResource(ProjectData* projectData, ResourceManagerV2* resourceManager, ResourceDirectory* directory, ResourceDirectory* newParent)
	{
		ResourceDirectory* dup = new ResourceDirectory();
		*dup				   = *directory;
		dup->children.clear();
		newParent->AddChild(dup);

		if (!directory->isFolder)
		{
			const String	 path		= projectData->GetResourcePath(directory->resourceID);
			const String	 duplicated = FileSystem::Duplicate(path);
			const ResourceID newID		= projectData->ConsumeResourceID();
			dup->resourceID				= newID;
			FileSystem::ChangeDirectoryName(duplicated, projectData->GetResourcePath(newID));

			Resource* res = static_cast<Resource*>(resourceManager->OpenResource(projectData, directory->resourceTID, newID, nullptr));
			res->SetID(newID);
			resourceManager->CloseResource(projectData, res, true);

			if (!directory->thumbnailBuffer.IsEmpty())
			{
				dup->thumbnailBuffer = RawStream();
				dup->thumbnailBuffer.Create(directory->thumbnailBuffer.GetRaw(), directory->thumbnailBuffer.GetSize());
				dup->_thumbnailAtlasImage = nullptr;
			}
		}

		for (ResourceDirectory* c : directory->children)
			DuplicateResource(projectData, resourceManager, c, dup);
	}

} // namespace Lina::Editor
