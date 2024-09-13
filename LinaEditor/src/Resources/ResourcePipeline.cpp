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

#include "Common/System/System.hpp"
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

	void ResourcePipeline::Initialize(Editor* editor)
	{
		m_editor = editor;

		if (!FileSystem::FileOrPathExists(GetResourceDirectory()))
			FileSystem::CreateFolderInPath(GetResourceDirectory());

		// Check if the actual resource file for a ResourceDirectory in the project exists, if not delete the directory.
		ResourceDirectory* root = &m_editor->GetProjectManager().GetProjectData()->GetResourceRoot();
		VerifyResources(root);
		m_editor->GetProjectManager().SaveProjectChanges();

		// Go through all the resource files in the cache directory.
		// If the equivalent resource is not included in/used by the project delete the file to prevent littering.
		const String   resDir = GetResourceDirectory();
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
	}

	void ResourcePipeline::VerifyResources(ResourceDirectory* dir)
	{
		Vector<ResourceDirectory*> killList;

		for (ResourceDirectory* c : dir->children)
		{
			if (!c->isFolder)
			{
				const String path = GetResourcePath(c->resourceID);
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
			dir->_thumbnailAtlasImage = m_editor->GetAtlasManager().AddImageToAtlas(thumbData.data(), Vector2ui(width, height), bytesPerPixel);
			delete[] thumbData.data();
		}

		for (ResourceDirectory* c : dir->children)
			GenerateThumbnailAtlases(c);
	}

	ResourceID ResourcePipeline::SaveNewResource(TypeID tid, uint32 subType)
	{
		const ResourceID id = m_editor->GetProjectManager().ConsumeResourceID();

		const String path = GetResourcePath(id);
		if (tid == GetTypeID<GUIWidget>())
		{
			GUIWidget w(id);
			w.GetRoot().GetWidgetProps().debugName = "Root";
			w.SaveToFileAsBinary(path);
		}
		else if (tid == GetTypeID<Material>())
		{
			Material mat(id);
			mat.SaveToFileAsBinary(path);
		}
		else if (tid == GetTypeID<EntityWorld>())
		{
			EntityWorld world(id);
			world.SaveToFileAsBinary(path);
		}
		else if (tid == GetTypeID<TextureSampler>())
		{
			TextureSampler sampler(id);
			sampler.SaveToFileAsBinary(path);
		}
		else if (tid == GetTypeID<Shader>())
		{
			Shader shader(id);

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
			PhysicsMaterial mat(id);
			mat.SaveToFileAsBinary(path);
		}

		return id;
	}

	void* ResourcePipeline::OpenResource(TypeID tid, ResourceID resourceID, void* subdata)
	{
		void* ptr = nullptr;

		const String path = GetResourceDirectory() + "Resource_" + TO_STRING(resourceID) + ".linaresource";

		if (!FileSystem::FileOrPathExists(path))
			return nullptr;

		if (tid == GetTypeID<GUIWidget>())
		{
			GUIWidget* w = new GUIWidget(0, subdata);
			w->LoadFromFile(path);
			ptr = w;
		}
		else if (tid == GetTypeID<Material>())
		{
			Material* m = new Material(0);
			m->LoadFromFile(path);
			ptr = m;
		}
		else if (tid == GetTypeID<EntityWorld>())
		{
			EntityWorld* w = new EntityWorld(0);
			w->LoadFromFile(path);
			ptr = w;
		}
		else if (tid == GetTypeID<Shader>())
		{
			Shader* s = new Shader(0);
			s->LoadFromFile(path);
			ptr = s;
		}
		else if (tid == GetTypeID<PhysicsMaterial>())
		{
			PhysicsMaterial* mat = new PhysicsMaterial(0);
			mat->LoadFromFile(path);
			ptr = mat;
		}
		else if (tid == GetTypeID<TextureSampler>())
		{
			TextureSampler* samp = new TextureSampler(0);
			samp->LoadFromFile(path);
			ptr = samp;
		}
		else if (tid == GetTypeID<Texture>())
		{
			Texture* txt	= new Texture(0);
			IStream	 stream = Serialization::LoadFromFile(path.c_str());
			txt->LoadFromStream(stream);
			stream.Destroy();
			ptr = txt;
		}
		else if (tid == GetTypeID<Font>())
		{
			Font*	fnt	   = new Font(0);
			IStream stream = Serialization::LoadFromFile(path.c_str());
			fnt->LoadFromStream(stream);
			stream.Destroy();
			ptr = fnt;
		}
		else if (tid == GetTypeID<Model>())
		{
			Model*	model  = new Model(0);
			IStream stream = Serialization::LoadFromFile(path.c_str());
			model->LoadFromStream(stream);
			stream.Destroy();
			ptr = model;
		}
		else if (tid == GetTypeID<Audio>())
		{
			Audio*	aud	   = new Audio(0);
			IStream stream = Serialization::LoadFromFile(path.c_str());
			aud->LoadFromStream(stream);
			stream.Destroy();
			ptr = aud;
		}
		else
		{
			LINA_ASSERT(false, "");
		}

		return ptr;
	}

	void ResourcePipeline::SaveResource(Resource* res)
	{
		res->SaveToFileAsBinary(GetResourcePath(res->GetID()).c_str());
	}

	String ResourcePipeline::GetResourceDirectory()
	{
		const String project = m_editor->GetProjectManager().GetProjectData()->GetPath();
		return FileSystem::GetFilePath(project) + "_LinaResourceCache/";
	}

	String ResourcePipeline::GetResourcePath(ResourceID id)
	{
		return GetResourceDirectory() + "Resource_" + TO_STRING(id) + ".linaresource";
	}

	void ResourcePipeline::ImportResources(ResourceDirectory* src, const Vector<String>& absPaths)
	{
		// Verify the paths, create resource directory for each verified path.
		Vector<String> validPaths;
		validPaths.reserve(absPaths.size());
		Vector<ResourceDirectory*> newChildren;
		newChildren.reserve(absPaths.size());

		for (const String& str : absPaths)
		{
			if (!FileSystem::FileOrPathExists(str))
				continue;
			const String   extension = FileSystem::GetFileExtension(str);
			const StringID extSid	 = TO_SID(extension);

			TypeID resourceTID = 0;

			if (extSid == "png"_hs || extSid == "jpg"_hs)
				resourceTID = GetTypeID<Texture>();
			else if (extSid == "mp3"_hs)
				resourceTID = GetTypeID<Audio>();
			else if (extSid == "glb"_hs || extSid == "gtlf"_hs)
				resourceTID = GetTypeID<Model>();
			else if (extSid == "otf"_hs || extSid == "ttf"_hs)
				resourceTID = GetTypeID<Font>();
			else
				continue;

			validPaths.push_back(str);

			ResourceDirectory* child = src->CreateChild({
				.isFolder	 = false,
				.resourceID	 = m_editor->GetProjectManager().GetProjectData()->ConsumeResourceID(),
				.resourceTID = resourceTID,
				.name		 = FileSystem::GetFilenameOnlyFromPath(str),
			});
			newChildren.push_back(child);
		}
		m_editor->GetProjectManager().SaveProjectChanges();

		// Fire up notification displayer in the main window.
		const float totalCount		  = static_cast<float>(validPaths.size());
		m_importedResourcesCount	  = 0;
		NotificationDesc notification = {
			.icon		= NotificationIcon::Loading,
			.title		= Locale::GetStr(LocaleStr::ImportingResources),
			.showButton = false,
			.onProgress = [totalCount, this](float& out) { out = static_cast<float>(m_importedResourcesCount.load()) / static_cast<float>(totalCount); },
		};
		m_editor->GetWindowPanelManager().GetNotificationDisplayer(m_editor->GetWindowPanelManager().GetMainWindow())->AddNotification(notification);

		// Import resources in parallel.
		Taskflow tf;
		tf.for_each_index(0, static_cast<int>(validPaths.size()), 1, [&](int i) {
			const String&	   path = validPaths.at(i);
			ResourceDirectory* dir	= newChildren.at(i);
			const ResourceID   id	= dir->resourceID;

			LinaGX::TextureBuffer thumbnail = {};

			if (dir->resourceTID == GetTypeID<Texture>())
			{
				Texture txt(id);
				txt.LoadFromFile(path);
				txt.SaveToFileAsBinary(GetResourcePath(id));
				thumbnail = ThumbnailGenerator::GenerateThumbnail(&txt);
			}
			else if (dir->resourceTID == GetTypeID<Font>())
			{
				Font font(id);
				font.LoadFromFile(path);
				font.SaveToFileAsBinary(GetResourcePath(id));
				thumbnail = ThumbnailGenerator::GenerateThumbnailFont(path);
			}
			else if (dir->resourceTID == GetTypeID<Audio>())
			{
				Audio aud(id);
				aud.LoadFromFile(path);
				aud.SaveToFileAsBinary(GetResourcePath(id));
				thumbnail = ThumbnailGenerator::GenerateThumbnail(&aud);
			}
			else if (dir->resourceTID == GetTypeID<Model>())
			{
				Model model(id);
				model.LoadFromFile(path);
				model.SaveToFileAsBinary(GetResourcePath(id));
				thumbnail = ThumbnailGenerator::GenerateThumbnail(&model);
			}

			if (thumbnail.pixels != nullptr)
			{
				OStream stream;
				stream << thumbnail.width << thumbnail.height << thumbnail.bytesPerPixel;
				stream.WriteRaw(thumbnail.pixels, thumbnail.width * thumbnail.height * thumbnail.bytesPerPixel);
				dir->_thumbnailAtlasImage = m_editor->GetAtlasManager().AddImageToAtlas(thumbnail.pixels, Vector2ui(thumbnail.width, thumbnail.height), thumbnail.bytesPerPixel);
				dir->thumbnailBuffer.Create(stream);
				delete[] thumbnail.pixels;
				stream.Destroy();
			}

			m_importedResourcesCount.fetch_add(1);
		});

		m_editor->GetSystem()->GetMainExecutor()->RunAndWait(tf);
		m_editor->GetProjectManager().SaveProjectChanges();
	}

	void ResourcePipeline::DuplicateResource(ResourceDirectory* directory, ResourceDirectory* newParent)
	{
		ResourceDirectory* dup = new ResourceDirectory();
		*dup				   = *directory;
		dup->children.clear();
		newParent->AddChild(dup);

		if (!directory->isFolder)
		{
			const String	 path		= GetResourcePath(directory->resourceID);
			const String	 duplicated = FileSystem::Duplicate(path);
			const ResourceID newID		= m_editor->GetProjectManager().ConsumeResourceID();
			dup->resourceID				= newID;
			FileSystem::ChangeDirectoryName(duplicated, GetResourcePath(newID));
			Resource* res = static_cast<Resource*>(OpenResource(directory->resourceTID, newID, nullptr));
			res->SetID(newID);
			CloseAndSaveResource(res);

			if (!directory->thumbnailBuffer.IsEmpty())
			{
				dup->thumbnailBuffer = RawStream();
				dup->thumbnailBuffer.Create(directory->thumbnailBuffer.GetRaw(), directory->thumbnailBuffer.GetSize());
				GenerateThumbnailAtlases(dup);
			}
		}

		for (ResourceDirectory* c : directory->children)
		{
			DuplicateResource(c, dup);
		}
	}
} // namespace Lina::Editor
