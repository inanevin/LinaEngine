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

#include "Common/System/System.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/Graphics/GfxManager.hpp"
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
	void ResourcePipeline::Initialize(Editor* editor)
	{
		m_editor = editor;

		ProjectData* projectData = m_editor->GetProjectManager().GetProjectData();

		if (!FileSystem::FileOrPathExists(projectData->GetResourceDirectory()))
			FileSystem::CreateFolderInPath(projectData->GetResourceDirectory());

		// Check if the actual resource file for a ResourceDirectory in the project exists, if not delete the directory.
		ResourceDirectory* root = &projectData->GetResourceRoot();
		VerifyResources(root);
		m_editor->GetProjectManager().SaveProjectChanges();

		// Go through all the resource files in the cache directory.
		// If the equivalent resource is not included in/used by the project delete the file to prevent littering.
		const String   resDir = projectData->GetResourceDirectory();
		Vector<String> files;
		FileSystem::GetFilesInDirectory(resDir, files);
		for (const String& file : files)
		{
			const String fileName = FileSystem::GetFilenameOnlyFromPath(file);
			const size_t under	  = fileName.find("_");
			if (under == String::npos)
				continue;
			const String	   resID	= fileName.substr(under + 1, fileName.length() - under);
			const ResourceID   resIDInt = static_cast<ResourceID>(UtilStr::StringToBigInt(resID));
			ResourceDirectory* found	= root->FindResource(resIDInt);
			if (found == nullptr)
				FileSystem::DeleteFileInPath(file);
		}

		GenerateThumbnailAtlases(root);
		m_editor->GetAtlasManager().RefreshDirtyAtlases();
	}

	void ResourcePipeline::VerifyResources(ResourceDirectory* dir)
	{
		ProjectData* projectData = m_editor->GetProjectManager().GetProjectData();

		Vector<ResourceDirectory*> killList;

		for (ResourceDirectory* c : dir->children)
		{
			if (!c->isFolder)
			{
				const String path = projectData->GetResourcePath(c->resourceID);
				if (!FileSystem::FileOrPathExists(path))
				{
					killList.push_back(c);
				}
			}
			else
				VerifyResources(c);
		}

		for (ResourceDirectory* d : killList)
			dir->DestroyChild(d);
	}

	void ResourcePipeline::GenerateThumbnailAtlases(ResourceDirectory* dir)
	{
		if (!dir->isFolder && !dir->thumbnailBuffer.IsEmpty())
		{
			IStream stream;
			stream.Create(dir->thumbnailBuffer.GetRaw(), dir->thumbnailBuffer.GetSize());
			uint32 width = 0, height = 0, bytesPerPixel = 0;
			stream >> width;
			stream >> height;
			stream >> bytesPerPixel;
			const size_t sz		   = static_cast<size_t>(width * height * bytesPerPixel);
			Span<uint8>	 thumbData = {new uint8[sz], sz};
			stream.ReadToRaw(thumbData);
			stream.Destroy();
			dir->_thumbnailAtlasImage = m_editor->GetAtlasManager().AddImageToAtlas(thumbData.data(), Vector2ui(width, height), bytesPerPixel == 1 ? LinaGX::Format::R8_UNORM : LinaGX::Format::R8G8B8A8_SRGB);
			delete[] thumbData.data();
		}

		for (ResourceDirectory* c : dir->children)
			GenerateThumbnailAtlases(c);
	}

	ResourceID ResourcePipeline::SaveNewResource(TypeID tid, uint32 subType)
	{
		const ResourceID id			 = m_editor->GetProjectManager().ConsumeResourceID();
		ProjectData*	 projectData = m_editor->GetProjectManager().GetProjectData();
		const String	 path		 = projectData->GetResourcePath(id);

		if (tid == GetTypeID<GUIWidget>())
		{
			GUIWidget w(id);
			w.GetRoot().GetWidgetProps().debugName = "Root";
			w.SaveToFileAsBinary(path);
		}
		else if (tid == GetTypeID<Material>())
		{
			Material mat(id, "");
			mat.SaveToFileAsBinary(path);
		}
		else if (tid == GetTypeID<EntityWorld>())
		{
			EntityWorld world(id, "");
			world.SaveToFileAsBinary(path);
		}
		else if (tid == GetTypeID<TextureSampler>())
		{
			TextureSampler sampler(id, "");
			sampler.SaveToFileAsBinary(path);
		}
		else if (tid == GetTypeID<Shader>())
		{
			Shader shader(id, "");

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
			PhysicsMaterial mat(id, "");
			mat.SaveToFileAsBinary(path);
		}

		return id;
	}

	void ResourcePipeline::ImportResources(ResourceDirectory* src, const Vector<String>& absPaths)
	{
		// Thumbnails.
		m_editor->GetGfxManager()->Join();

		// Fire up notification displayer in the main window.
		const float totalCount		  = static_cast<float>(absPaths.size());
		m_importedResourcesCount	  = 0;
		NotificationDesc notification = {
			.icon		= NotificationIcon::Loading,
			.title		= Locale::GetStr(LocaleStr::ImportingResources),
			.showButton = false,
			.onProgress = [totalCount, this](float& out) { out = static_cast<float>(m_importedResourcesCount.load()) / static_cast<float>(totalCount); },
		};
		m_editor->GetWindowPanelManager().GetNotificationDisplayer(m_editor->GetWindowPanelManager().GetMainWindow())->AddNotification(notification);

		ProjectData* projectData = m_editor->GetProjectManager().GetProjectData();

		// Import resources in parallel.
		Taskflow tf;

		tf.emplace([absPaths, this, projectData, src]() {
			Shader* defaultShader = nullptr;

			for (const String& path : absPaths)
			{
				if (!FileSystem::FileOrPathExists(path))
				{
					m_importedResourcesCount.fetch_add(1);
					continue;
				}

				const String   extension = FileSystem::GetFileExtension(path);
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
				else
				{
					m_importedResourcesCount.fetch_add(1);
					continue;
				}

				ResourceDirectory* dir = src->CreateChild({
					.isFolder	 = false,
					.resourceID	 = m_editor->GetProjectManager().GetProjectData()->ConsumeResourceID(),
					.resourceTID = resourceTID,
					.name		 = FileSystem::GetFilenameOnlyFromPath(path) + "." + FileSystem::GetFileExtension(path),
				});

				const ResourceID id = dir->resourceID;
				if (dir->resourceTID == GetTypeID<Texture>())
				{
					Texture txt(id, dir->name);
					txt.SetPath(path);
					txt.LoadFromFile(path);
					txt.SaveToFileAsBinary(projectData->GetResourcePath(id));
					GenerateThumbnailForResource(dir, &txt, false);
				}
				else if (dir->resourceTID == GetTypeID<Font>())
				{
					Font font(id, dir->name);
					font.SetPath(path);
					font.LoadFromFile(path);
					font.SaveToFileAsBinary(projectData->GetResourcePath(id));
					GenerateThumbnailForResource(dir, &font, false);
				}
				else if (dir->resourceTID == GetTypeID<Audio>())
				{
					Audio aud(id, dir->name);
					aud.SetPath(path);
					aud.LoadFromFile(path);
					aud.SaveToFileAsBinary(projectData->GetResourcePath(id));
					GenerateThumbnailForResource(dir, &aud, false);
				}
				else if (dir->resourceTID == GetTypeID<Model>())
				{
					Model model(id, dir->name);
					model.SetPath(path);
					model.LoadFromFile(path);
					model.SaveToFileAsBinary(projectData->GetResourcePath(id));
					GenerateThumbnailForResource(dir, &model, false);

					if (defaultShader == nullptr)
					{
						defaultShader = new Shader(0, "");
						defaultShader->LoadFromFile(DEFAULT_SHADER_OBJECT_PATH);
					}

					const Vector<ModelMaterial>&	   matDefs	   = model.GetMaterialDefs();
					const Vector<Model::ModelTexture>& textureDefs = model.GetTextureDefs();

					for (const Model::ModelTexture& def : textureDefs)
					{
						const ResourceID newID = m_editor->GetProjectManager().GetProjectData()->ConsumeResourceID();

						ResourceDirectory* child = dir->parent->CreateChild({
							.isFolder	 = false,
							.resourceID	 = newID,
							.resourceTID = GetTypeID<Texture>(),
							.name		 = def.name,
						});

						Texture texture(newID, def.name);
						texture.LoadFromBuffer(def.buffer.pixels, def.buffer.width, def.buffer.height, def.buffer.bytesPerPixel);
						texture.SaveToFileAsBinary(projectData->GetResourcePath(newID));
						GenerateThumbnailForResource(child, &texture, false);
					}

					for (const ModelMaterial& def : matDefs)
					{
						const ResourceID newID = m_editor->GetProjectManager().GetProjectData()->ConsumeResourceID();

						ResourceDirectory* child = dir->parent->CreateChild({
							.isFolder	 = false,
							.resourceID	 = newID,
							.resourceTID = GetTypeID<Material>(),
							.name		 = def.name,
						});

						Material mat(newID, def.name);
						mat.SetShader(defaultShader);
						mat.SetProperty("color"_hs, def.albedo);
						mat.SaveToFileAsBinary(projectData->GetResourcePath(newID));
						GenerateThumbnailForResource(child, &mat, false);
					}

					model.DestroyTextureDefs();
				}

				m_importedResourcesCount.fetch_add(1);
			}

			DockArea* outDockArea = nullptr;
			Panel*	  p			  = m_editor->GetWindowPanelManager().FindPanelOfType(PanelType::ResourceBrowser, 0, outDockArea);
			if (p)
				static_cast<PanelResourceBrowser*>(p)->GetBrowser()->RefreshDirectory();

			if (defaultShader)
				delete defaultShader;

			m_editor->GetProjectManager().SaveProjectChanges();
			m_editor->GetAtlasManager().RefreshDirtyAtlases()
		});

		m_executor.RunMove(tf);
	}

	void ResourcePipeline::GenerateThumbnailForResource(ResourceDirectory* dir, Resource* resource, bool refreshAtlases)
	{
		const ResourceID id = dir->resourceID;

		if (!dir->thumbnailBuffer.IsEmpty())
			dir->thumbnailBuffer.Destroy();

		if (dir->_thumbnailAtlasImage != nullptr)
		{
			m_editor->GetAtlasManager().RemoveImage(dir->_thumbnailAtlasImage);
			dir->_thumbnailAtlasImage = nullptr;
		}

		LinaGX::TextureBuffer thumbnail = {};
		thumbnail						= ThumbnailGenerator::GenerateThumbnailForResource(resource);

		if (thumbnail.pixels != nullptr)
		{
			OStream stream;
			stream << thumbnail.width << thumbnail.height << thumbnail.bytesPerPixel;
			stream.WriteRaw(thumbnail.pixels, thumbnail.width * thumbnail.height * thumbnail.bytesPerPixel);
			dir->_thumbnailAtlasImage = m_editor->GetAtlasManager().AddImageToAtlas(thumbnail.pixels, Vector2ui(thumbnail.width, thumbnail.height), thumbnail.bytesPerPixel == 1 ? LinaGX::Format::R8_UNORM : LinaGX::Format::R8G8B8A8_SRGB);
			dir->thumbnailBuffer.Create(stream);
			delete[] thumbnail.pixels;
			stream.Destroy();
		}

		if (refreshAtlases)
			m_editor->GetAtlasManager().RefreshDirtyAtlases();
	}

	void ResourcePipeline::DuplicateResource(ResourceManagerV2* resourceManager, ResourceDirectory* directory, ResourceDirectory* newParent)
	{
		ResourceDirectory* dup = new ResourceDirectory();
		*dup				   = *directory;
		dup->children.clear();
		newParent->AddChild(dup);

		if (!directory->isFolder)
		{
			ProjectData*	 projectData = m_editor->GetProjectManager().GetProjectData();
			const String	 path		 = projectData->GetResourcePath(directory->resourceID);
			const String	 duplicated	 = FileSystem::Duplicate(path);
			const ResourceID newID		 = m_editor->GetProjectManager().ConsumeResourceID();
			dup->resourceID				 = newID;
			FileSystem::ChangeDirectoryName(duplicated, projectData->GetResourcePath(newID));

			Resource* res = static_cast<Resource*>(resourceManager->OpenResource(projectData, directory->resourceTID, newID, nullptr));
			res->SetID(newID);
			resourceManager->CloseResource(projectData, res, true);

			if (!directory->thumbnailBuffer.IsEmpty())
			{
				dup->thumbnailBuffer = RawStream();
				dup->thumbnailBuffer.Create(directory->thumbnailBuffer.GetRaw(), directory->thumbnailBuffer.GetSize());
				GenerateThumbnailAtlases(dup);
			}
		}

		for (ResourceDirectory* c : directory->children)
		{
			DuplicateResource(resourceManager, c, dup);
		}
	}

} // namespace Lina::Editor
