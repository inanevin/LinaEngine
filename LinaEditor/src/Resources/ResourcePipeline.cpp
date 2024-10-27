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
			return nullptr;
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

		ResourceDirectory* newCreated = src->CreateChild({
			.name		  = name,
			.isFolder	  = false,
			.resourceID	  = id,
			.resourceTID  = tid,
			.resourceType = ResourceType::EngineCreated,
		});

		LinaGX::TextureBuffer thumb = {};

		if (tid == GetTypeID<GUIWidget>())
		{
			GUIWidget w(id, name);
			w.GetRoot().GetWidgetProps().debugName = "Root";
			w.SaveToFileAsBinary(path);
		}
		else if (tid == GetTypeID<Material>())
		{
			Material		 mat(id, name);
			const ResourceID targetShader = subType == 0 ? EDITOR_SHADER_DEFAULT_OPAQUE_SURFACE_ID : subType;

			IStream shaderStream = Serialization::LoadFromFile(project->GetResourcePath(targetShader).c_str());

			if (shaderStream.Empty())
			{
				LINA_ERR("Failed saving new material!");
				src->DestroyChild(newCreated);
				return nullptr;
			}

			Shader sh(0, "");
			sh.LoadFromStream(shaderStream);
			mat.SetShader(&sh);

			// Try set defaults
			for (MaterialProperty* prop : mat.GetProperties())
				TrySetMaterialProperty(prop);

			mat.SaveToFileAsBinary(path);
			shaderStream.Destroy();
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

			const String savePath = PlatformProcess::SaveDialog({
				.title		   = Locale::GetStr(LocaleStr::CreateANewShader),
				.primaryButton = Locale::GetStr(LocaleStr::Create),
				.extensions	   = {"linashader"},
				.mode		   = PlatformProcess::DialogMode::SelectFile,
			});

			// Load default text.
			if (subType == 0)
			{
				// Opaque surface
				const String shaderTxt = FileSystem::ReadFileContentsAsString(EDITOR_SHADER_DEFAULT_OPAQUE_SURFACE_PATH);
				Serialization::WriteToFile(shaderTxt, savePath);
				shader.GetMeta().shaderType = ShaderType::OpaqueSurface;
			}
			else if (subType == 1)
			{
				// Transparent surface
				const String shaderTxt = FileSystem::ReadFileContentsAsString(EDITOR_SHADER_DEFAULT_TRANSPARENT_SURFACE_PATH);
				Serialization::WriteToFile(shaderTxt, savePath);
				shader.GetMeta().shaderType = ShaderType::TransparentSurface;
			}
			else if (subType == 2)
			{
				// Sky
				const String shaderTxt = FileSystem::ReadFileContentsAsString(EDITOR_SHADER_DEFAULT_SKY_PATH);
				Serialization::WriteToFile(shaderTxt, savePath);
				shader.GetMeta().shaderType = ShaderType::Sky;
			}
			else if (subType == 3)
			{
				// Post process
				const String shaderTxt = FileSystem::ReadFileContentsAsString(EDITOR_SHADER_DEFAULT_POSTPROCESS_PATH);
				Serialization::WriteToFile(shaderTxt, savePath);
				shader.GetMeta().shaderType = ShaderType::PostProcess;
			}

			shader.SetPath(savePath);
			shader.SetName(FileSystem::GetFilenameOnlyFromPath(savePath));
			shader.LoadFromFile(savePath);
			shader.SaveToFileAsBinary(path);
			newCreated->name = shader.GetName();
		}
		else if (tid == GetTypeID<PhysicsMaterial>())
		{
			PhysicsMaterial mat(id, name);
			mat.SaveToFileAsBinary(path);
		}

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
				dir = src->CreateChild({
					.name						 = name,
					.sourcePathRelativeToProject = FileSystem::GetRelative(projectRoot, def.path),
					.isFolder					 = false,
					.resourceID					 = id,
					.resourceTID				 = resourceTID,
					.resourceType				 = ResourceType::ExternalSource,
				});

				createdDirs.push_back(dir);
			};

			if (resourceTID == GetTypeID<Shader>())
			{
				Shader shader(0, name);

				if (def.subType == 0)
					shader.GetMeta().shaderType = ShaderType::OpaqueSurface;
				else if (def.subType == 1)
					shader.GetMeta().shaderType = ShaderType::TransparentSurface;
				else if (def.subType == 2)
					shader.GetMeta().shaderType = ShaderType::Sky;
				else if (def.subType == 3)
					shader.GetMeta().shaderType = ShaderType::PostProcess;
				else if (def.subType == 4)
					shader.GetMeta().shaderType = ShaderType::Lighting;

				if (!shader.LoadFromFile(def.path))
					continue;

				shader.SetID(def.id == 0 ? projectData->ConsumeResourceID() : def.id);
				shader.SetPath(def.path);
				shader.SaveToFileAsBinary(projectData->GetResourcePath(shader.GetID()));
				createDirectory(shader.GetID());
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
					defaultShader->LoadFromFile(EDITOR_SHADER_DEFAULT_OPAQUE_SURFACE_PATH);
				}

				const Vector<ModelMaterial>&	   matDefs	   = model.GetMaterialDefs();
				const Vector<Model::ModelTexture>& textureDefs = model.GetTextureDefs();

				Vector<ResourceID> createdTextures;
				Vector<String>	   createdTextureNames;

				for (const Model::ModelTexture& def : textureDefs)
				{
					const ResourceID newID = projectData->ConsumeResourceID();

					ResourceDirectory* child = dir->parent->CreateChild({
						.name		 = def.name,
						.isFolder	 = false,
						.resourceID	 = newID,
						.resourceTID = GetTypeID<Texture>(),
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

					ResourceDirectory* child = dir->parent->CreateChild({
						.name		 = def.name,
						.isFolder	 = false,
						.resourceID	 = newID,
						.resourceTID = GetTypeID<Material>(),
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
					mat.SetProperty("txtAlbedo"_hs, albedo);
					mat.SetProperty("txtNormal"_hs, normal);
					mat.SaveToFileAsBinary(projectData->GetResourcePath(newID));
					matIdx++;
				}
				model.SaveToFileAsBinary(projectData->GetResourcePath(model.GetID()));
				model.DestroyTextureDefs();
			}
		}

		if (defaultShader)
			delete defaultShader;

		onProgress(sz, {}, true);
		return createdDirs;
	}

	void ResourcePipeline::DuplicateResource(ProjectManager& projectManager, ResourceManagerV2* resourceManager, ResourceDirectory* directory, ResourceDirectory* newParent)
	{
		ProjectData* projectData = projectManager.GetProjectData();

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
			projectManager.AddToThumbnailQueue(dup->resourceID);
		}

		for (ResourceDirectory* c : directory->children)
			DuplicateResource(projectManager, resourceManager, c, dup);
	}

} // namespace Lina::Editor
