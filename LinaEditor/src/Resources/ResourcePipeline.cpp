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
#include "Editor/Resources/ShaderImport.hpp"

#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/Platform/PlatformProcess.hpp"
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
#include "Editor/World/WorldUtility.hpp"

#include <LinaGX/Utility/ImageUtility.hpp>
#include <LinaGX/Utility/ModelUtility.hpp>

namespace Lina::Editor
{
	void ResourcePipeline::TrySetMaterialProperty(MaterialProperty* prop)
	{
		if (prop->propDef.type == ShaderPropertyType::Vec4)
		{
			const String lowerName = UtilStr::ToLower(prop->propDef.name);

			if (lowerName.find("tiling") != String::npos || lowerName.find("uv") != String::npos)
			{
				Vector4 data = Vector4(1.0f, 1.0f, 0.0f, 0.0f);
				MEMCPY(prop->data.data(), &data, sizeof(Vector4));
			}
			else if (lowerName.find("color") != String::npos)
			{
				Vector4 data = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
				MEMCPY(prop->data.data(), &data, sizeof(Vector4));
			}
		}
		else if (prop->propDef.type == ShaderPropertyType::Texture2D)
		{
			ResourceID textureID = EDITOR_TEXTURE_EMPTY_ALBEDO_ID;

			const String lowerName = UtilStr::ToLower(prop->propDef.name);

			if (lowerName.find("normal") != String::npos)
				textureID = EDITOR_TEXTURE_EMPTY_NORMAL_ID;
			else if (lowerName.find("ao") != String::npos)
				textureID = EDITOR_TEXTURE_EMPTY_AO_ID;
			else if (lowerName.find("metallic") != String::npos || lowerName.find("roughness") != String::npos)
				textureID = EDITOR_TEXTURE_EMPTY_METALLIC_ROUGHNESS_ID;
			else if (lowerName.find("emissive") != String::npos)
				textureID = EDITOR_TEXTURE_EMPTY_EMISSIVE_ID;

			const LinaTexture2D txt = {
				.texture = textureID,
				.sampler = EDITOR_SAMPLER_DEFAULT_ID,
			};

			MEMCPY(prop->data.data(), &txt, sizeof(LinaTexture2D));
		}
	}

	void ResourcePipeline::ChangeMaterialShader(ProjectData* project, Material* material, Shader* shader)
	{

		const Vector<ShaderPropertyDefinition>& propDefs   = shader->GetPropertyDefinitions();
		const Vector<MaterialProperty*>			properties = material->GetProperties();

		// These are the new properties that'll be added to the material, we will not modify the existing ones.
		Vector<StringID> newProperties;
		for (const ShaderPropertyDefinition& def : propDefs)
		{
			auto it = linatl::find_if(properties.begin(), properties.end(), [def](MaterialProperty* p) -> bool { return p->propDef == def; });
			if (it == properties.end())
				newProperties.push_back(def.sid);
		}

		material->SetShader(shader);
		const Vector<MaterialProperty*> currentMaterialProperties = material->GetProperties();
		for (StringID prop : newProperties)
		{

			auto it = linatl::find_if(currentMaterialProperties.begin(), currentMaterialProperties.end(), [prop](MaterialProperty* p) -> bool { return p->propDef.sid == prop; });
			if (it != currentMaterialProperties.end())
				TrySetMaterialProperty(*it);
		}
	}

	void ResourcePipeline::ChangeMaterialShader(ProjectData* project, Material* material, ResourceID newShader)
	{
		IStream shaderStream = Serialization::LoadFromFile(project->GetResourcePath(newShader).c_str());

		if (shaderStream.Empty())
		{
			LINA_ERR("Failed changing material shader!");
			return;
		}

		Shader sh(0, "");
		sh.LoadFromStream(shaderStream);
		ChangeMaterialShader(project, material, &sh);

		shaderStream.Destroy();
	}

	ResourceDirectory* ResourcePipeline::SaveNewResource(ProjectData* project, ResourceDirectory* src, const String& name, TypeID tid, ResourceID inID, ResourceID subType)
	{
		const ResourceID id	  = inID != 0 ? inID : project->ConsumeResourceID();
		const String	 path = project->GetResourcePath(id);

		ResourceDirectory* newCreated = project->CreateResourceDirectory(src,
																		 {
																			 .name			  = name,
																			 .isFolder		  = false,
																			 .resourceID	  = id,
																			 .resourceTID	  = tid,
																			 .resourceType	  = tid == GetTypeID<Shader>() ? ResourceType::ExternalSource : ResourceType::EngineCreated,
																			 .lastModifiedSID = 0,
																		 });

		LinaGX::TextureBuffer thumb = {};

		auto saveDefault = [&]() {
			MetaType* meta = ReflectionSystem::Get().Resolve(tid);
			Resource* res  = static_cast<Resource*>(meta->GetFunction<void*()>("Allocate"_hs)());
			res->SetName(name);
			res->SetPath(path);
			res->SetID(id);
			res->SaveToFileAsBinary(path);
			meta->GetFunction<void(void*)>("Deallocate"_hs)(res);
		};

		if (tid == GetTypeID<Material>())
		{
			Material		 mat(id, name);
			const ResourceID targetShader = subType == 0 ? EDITOR_SHADER_DEFAULT_OPAQUE_SURFACE_ID : subType;

			IStream shaderStream = Serialization::LoadFromFile(project->GetResourcePath(targetShader).c_str());

			if (shaderStream.Empty())
			{
				LINA_ERR("Failed saving new material!");
				project->DestroyResourceDirectory(newCreated);
				return nullptr;
			}

			Shader sh(0, "");
			sh.LoadFromStream(shaderStream);
			mat.SetShader(&sh);

			// Try set defaults
			for (MaterialProperty* prop : mat.GetProperties())
				TrySetMaterialProperty(prop);

			if (mat.GetID() == EDITOR_MATERIAL_DEFAULT_SKY_ID)
				WorldUtility::SetupDefaultSkyMaterial(&mat, nullptr);

			mat.SaveToFileAsBinary(path);
			shaderStream.Destroy();
		}
		else if (tid == GetTypeID<EntityWorld>())
		{
			EntityWorld world(id, name);
			world.SetPath(path);
			world.GetGfxSettings().lightingMaterial = EDITOR_MATERIAL_DEFAULT_LIGHTING_ID;
			world.GetGfxSettings().skyMaterial		= EDITOR_MATERIAL_DEFAULT_SKY_ID;
			world.GetGfxSettings().skyModel			= EDITOR_MODEL_SKYSPHERE_ID;
			world.SaveToFileAsBinary(path);
		}
		else
			saveDefault();

		return newCreated;
	}

	Vector<ResourceDirectory*> ResourcePipeline::ImportResources(ProjectData* projectData, ResourceDirectory* src, const Vector<ResourceImportDef>& defs, Delegate<void(uint32 imported, const ResourceImportDef& importDef, bool isCompleted)> onProgress)
	{
		Shader*					   defaultShader = nullptr;
		Vector<ResourceDirectory*> createdDirs;

		const String projectRoot = FileSystem::GetFilePath(projectData->GetPath());

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

			const String	   name = FileSystem::GetFilenameOnlyFromPath(def.path);
			ResourceDirectory* dir	= nullptr;

			auto createDirectory = [&](ResourceID id) {
				dir = projectData->CreateResourceDirectory(src,
														   {
															   .name						= name,
															   .sourcePathRelativeToProject = FileSystem::GetRelative(projectRoot, def.path),
															   .isFolder					= false,
															   .resourceID					= id,
															   .resourceTID					= resourceTID,
															   .resourceType				= ResourceType::ExternalSource,
															   .lastModifiedSID				= TO_SID(FileSystem::GetLastModifiedDate(def.path)),
														   });

				createdDirs.push_back(dir);
			};

			auto loadDefault = [&]() {
				MetaType* meta = ReflectionSystem::Get().Resolve(resourceTID);
				Resource* res  = static_cast<Resource*>(meta->GetFunction<void*()>("Allocate"_hs)());
				if (res->LoadFromFile(def.path))
				{
					res->SetName(name);
					res->SetID(def.id == 0 ? projectData->ConsumeResourceID() : def.id);
					res->SetPath(def.path);
					res->SaveToFileAsBinary(projectData->GetResourcePath(res->GetID()));
					createDirectory(res->GetID());
				}
				meta->GetFunction<void(void*)>("Deallocate"_hs)(res);
			};

			if (resourceTID == GetTypeID<Shader>())
			{
				Shader shader(0, "");
				shader.SetName(name);
				shader.SetID(def.id == 0 ? projectData->ConsumeResourceID() : def.id);
				shader.SetPath(def.path);
				if (ShaderImport::ImportShader(&shader, def.path))
				{
					shader.SaveToFileAsBinary(projectData->GetResourcePath(shader.GetID()));
					createDirectory(shader.GetID());
				}
			}
			else if (resourceTID == GetTypeID<Model>())
			{
				Model model(0, name);
				if (!model.LoadFromFile(def.path))
					continue;

				model.SetID(def.id == 0 ? projectData->ConsumeResourceID() : def.id);
				model.SetPath(def.path);
				createDirectory(model.GetID());

				if (defaultShader == nullptr)
				{
					defaultShader = new Shader(EDITOR_SHADER_DEFAULT_OPAQUE_SURFACE_ID, EDITOR_SHADER_DEFAULT_OPAQUE_SURFACE_PATH);
					if (ShaderImport::ImportShader(defaultShader, EDITOR_SHADER_DEFAULT_OPAQUE_SURFACE_PATH))
					{
					}
				}

				const Vector<ModelMaterial>&	   matDefs	   = model.GetMaterialDefs();
				const Vector<Model::ModelTexture>& textureDefs = model.GetTextureDefs();

				Vector<ResourceID> createdTextures;
				Vector<String>	   createdTextureNames;

				for (const Model::ModelTexture& def : textureDefs)
				{
					const ResourceID newID = projectData->ConsumeResourceID();

					ResourceDirectory* child = projectData->CreateResourceDirectory(dir->parent,
																					{
																						.name		  = def.name,
																						.isFolder	  = false,
																						.resourceID	  = newID,
																						.resourceTID  = GetTypeID<Texture>(),
																						.resourceType = ResourceType::EngineCreated,
																					});

					const String lowerName = UtilStr::ToLower(def.name);
					bool		 isColor   = true;
					if (lowerName.find("normal") != String::npos || lowerName.find("roughness") != String::npos || lowerName.find("metallic") != String::npos)
						isColor = false;

					Texture texture(newID, def.name);
					texture.GetMeta().format	= isColor ? LinaGX::Format::R8G8B8A8_SRGB : LinaGX::Format::R8G8B8A8_UNORM;
					texture.GetMeta().force8Bit = true;
					texture.LoadFromBuffer(def.buffer.pixels, def.buffer.width, def.buffer.height, def.buffer.bytesPerPixel);
					texture.SaveToFileAsBinary(projectData->GetResourcePath(newID));

					createdTextures.push_back(newID);
					createdTextureNames.push_back(def.name);
				}

				Model::Metadata& meta = model.GetMeta();

				int32 matIdx = 0;
				for (const ModelMaterial& def : matDefs)
				{
					const ResourceID newID = projectData->ConsumeResourceID();

					ResourceDirectory* child = projectData->CreateResourceDirectory(dir->parent,
																					{
																						.name		  = def.name,
																						.isFolder	  = false,
																						.resourceID	  = newID,
																						.resourceTID  = GetTypeID<Material>(),
																						.resourceType = ResourceType::EngineCreated,
																					});

					meta.materials[matIdx] = newID;

					LinaTexture2D albedo = {
						.texture = EDITOR_TEXTURE_EMPTY_ALBEDO_ID,
						.sampler = EDITOR_SAMPLER_DEFAULT_ID,
					};

					LinaTexture2D normal = {
						.texture = EDITOR_TEXTURE_EMPTY_NORMAL_ID,
						.sampler = EDITOR_SAMPLER_DEFAULT_ID,
					};

					LinaTexture2D metallicRoughness = {
						.texture = EDITOR_TEXTURE_EMPTY_METALLIC_ROUGHNESS_ID,
						.sampler = EDITOR_SAMPLER_DEFAULT_ID,
					};

					LinaTexture2D ao = {
						.texture = EDITOR_TEXTURE_EMPTY_AO_ID,
						.sampler = EDITOR_SAMPLER_DEFAULT_ID,
					};

					LinaTexture2D emissive = {
						.texture = EDITOR_TEXTURE_EMPTY_EMISSIVE_ID,
						.sampler = EDITOR_SAMPLER_DEFAULT_ID,
					};

					for (auto [textureType, textureIndex] : def.textureIndices)
					{
						if (textureType == static_cast<uint8>(LinaGX::GLTFTextureType::BaseColor))
						{
							albedo.texture = createdTextures[textureIndex];
							continue;
						}

						if (textureType == static_cast<uint8>(LinaGX::GLTFTextureType::Normal))
						{
							normal.texture = createdTextures[textureIndex];
							continue;
						}

						if (textureType == static_cast<uint8>(LinaGX::GLTFTextureType::MetallicRoughness))
						{
							metallicRoughness.texture = createdTextures[textureIndex];
							continue;
						}

						if (textureType == static_cast<uint8>(LinaGX::GLTFTextureType::Occlusion))
						{
							ao.texture = createdTextures[textureIndex];
							continue;
						}

						if (textureType == static_cast<uint8>(LinaGX::GLTFTextureType::Emissive))
						{
							emissive.texture = createdTextures[textureIndex];
							continue;
						}
					}

					Material mat(newID, def.name);
					mat.SetShader(defaultShader);
					mat.SetProperty("color"_hs, def.albedo);
					mat.SetProperty("tilingAndOffset"_hs, Vector4(1, 1, 0, 0));
					mat.SetProperty("txtAlbedo"_hs, albedo);
					mat.SetProperty("txtNormal"_hs, normal);
					mat.SaveToFileAsBinary(projectData->GetResourcePath(newID));
					matIdx++;
				}
				model.SaveToFileAsBinary(projectData->GetResourcePath(model.GetID()));
				model.DestroyTextureDefs();
			}
			else
			{
				loadDefault();
			}
		}

		if (defaultShader)
			delete defaultShader;

		onProgress(sz, {}, true);
		return createdDirs;
	}

	void ResourcePipeline::DuplicateResource(Editor* editor, ResourceDirectory* directory, ResourceDirectory* newParent)
	{
		ProjectData*	   projectData = editor->GetProjectManager().GetProjectData();
		ResourceDirectory* dup		   = projectData->DuplicateResourceDirectory(newParent, directory);
		dup->children.clear();

		if (!directory->isFolder)
		{
			const String	 path		= projectData->GetResourcePath(directory->resourceID);
			const String	 duplicated = FileSystem::Duplicate(path);
			const ResourceID newID		= projectData->ConsumeResourceID();
			dup->resourceID				= newID;
			FileSystem::ChangeDirectoryName(duplicated, projectData->GetResourcePath(newID));

			MetaType* refMeta = ReflectionSystem::Get().Resolve(directory->resourceTID);

			Resource* res	 = static_cast<Resource*>(refMeta->GetFunction<void*()>("Allocate"_hs)());
			IStream	  stream = Serialization::LoadFromFile(projectData->GetResourcePath(newID).c_str());
			if (!stream.Empty())
			{
				res->LoadFromStream(stream);
				res->SetID(newID);
				res->SaveToFileAsBinary(projectData->GetResourcePath(newID));
			}
			refMeta->GetFunction<void(void* ptr)>("Deallocate"_hs)(res);

			stream.Destroy();
			TextureAtlasImage* img = ThumbnailGenerator::GenerateThumbnail(editor->GetProjectManager().GetProjectData(), dup->resourceID, dup->resourceTID, editor->GetAtlasManager());
			editor->GetProjectManager().SetThumbnail(dup, img);
		}

		for (ResourceDirectory* c : directory->children)
			DuplicateResource(editor, c, dup);
	}

} // namespace Lina::Editor
