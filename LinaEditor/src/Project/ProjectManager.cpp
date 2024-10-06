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

#include "Editor/Project/ProjectManager.hpp"
#include "Editor/Graphics/SurfaceRenderer.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Widgets/FX/ProgressCircleFill.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Widgets/EditorRoot.hpp"
#include "Editor/Resources/ResourcePipeline.hpp"
#include "Editor/IO/ThumbnailGenerator.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/Physics/PhysicsMaterial.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Platform/PlatformProcess.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/Application.hpp"

namespace Lina::Editor
{

	void ProjectManager::Initialize(Editor* editor)
	{
		m_editor = editor;

		m_primaryWidgetManager = &editor->GetWindowPanelManager().GetSurfaceRenderer(LINA_MAIN_SWAPCHAIN)->GetWidgetManager();

		const EditorSettings& settings = m_editor->GetSettings();

		if (FileSystem::FileOrPathExists(settings.GetLastProjectPath()))
			OpenProject(settings.GetLastProjectPath());
		else
		{

			Vector<CommonWidgets::GenericPopupButton> buttons = {
				{
					.title = Locale::GetStr(LocaleStr::Open),
					.onPressed =
						[this]() {
							const Vector<String> projectPaths = PlatformProcess::OpenDialog({
								.title				   = Locale::GetStr(LocaleStr::OpenExistingProject),
								.primaryButton		   = Locale::GetStr(LocaleStr::Open),
								.extensionsDescription = "",
								.extensions			   = {"linaproject"},
								.mode				   = PlatformProcess::DialogMode::SelectFile,
							});

							if (!projectPaths.empty())
							{
								m_editor->GetWindowPanelManager().UnlockAllForegrounds();
								OpenProject(projectPaths.front());
							}
						},
				},
				{
					.title = Locale::GetStr(LocaleStr::Create),
					.onPressed =
						[this]() {
							const String path = PlatformProcess::SaveDialog({
								.title				   = Locale::GetStr(LocaleStr::CreateNewProject),
								.primaryButton		   = Locale::GetStr(LocaleStr::Create),
								.extensionsDescription = "",
								.extensions			   = {"linaproject"},
							});

							if (!path.empty())
							{
								m_editor->GetWindowPanelManager().UnlockAllForegrounds();
								CreateEmptyProjectAndOpen(path);
							}
						},
				},
			};

			Widget* lock = m_editor->GetWindowPanelManager().LockAllForegrounds(m_primaryWidgetManager->GetRoot()->GetWindow(), Locale::GetStr(LocaleStr::WorkInProgressInAnotherWindow));

			lock->AddChild(CommonWidgets::BuildGenericPopupWithButtons(lock, Locale::GetStr(LocaleStr::NoProjectFound), buttons));
		}
	}

	void ProjectManager::PreTick()
	{
		TaskRunner::Poll();

		// if(m_frameCtrForAtlases > 60 && !m_atlasGenWorking.load())
		// {
		//     m_frameCtrForAtlases = 0;
		//     m_atlasGenWorking.store(true);
		//
		//     Taskflow tf;
		//     tf.emplace([this](){
		//         GenerateMissingAtlases(&m_currentProject->GetResourceRoot());
		//         TaskRunner::QueueTask([this](){
		//             m_frameCtrForAtlases = 0;
		//             m_atlasGenWorking.store(false);
		//             Application::GetLGX()->Join();
		//             m_editor->GetAtlasManager().RefreshPoolAtlases();
		//         });
		//     });
		//     m_executor.RunMove(tf);
		// }
		//
		// m_frameCtrForAtlases++;
	}

	void ProjectManager::Shutdown()
	{
		RemoveCurrentProject();
	}

	void ProjectManager::OpenDialogCreateProject()
	{
		const String path = PlatformProcess::SaveDialog({
			.title				   = Locale::GetStr(LocaleStr::CreateNewProject),
			.primaryButton		   = Locale::GetStr(LocaleStr::Create),
			.extensionsDescription = "",
			.extensions			   = {"linaproject"},
		});

		if (!path.empty())
		{
			CreateEmptyProjectAndOpen(path);
		}
	}

	void ProjectManager::OpenDialogSelectProject()
	{
		const Vector<String> projectPaths = PlatformProcess::OpenDialog({
			.title				   = Locale::GetStr(LocaleStr::OpenExistingProject),
			.primaryButton		   = Locale::GetStr(LocaleStr::Open),
			.extensionsDescription = "",
			.extensions			   = {"linaproject"},
			.mode				   = PlatformProcess::DialogMode::SelectFile,
		});

		if (!projectPaths.empty())
		{
			OpenProject(projectPaths.front());
		}
	}

	void ProjectManager::SaveProjectChanges()
	{
		m_currentProject->SetDirty(false);
		m_currentProject->SaveToFile();
	}

	ResourceID ProjectManager::ConsumeResourceID()
	{
		const ResourceID id = m_currentProject->ConsumeResourceID();
		SaveProjectChanges();
		return id;
	}

	void ProjectManager::AddListener(ProjectManagerListener* listener)
	{
		m_listeners.push_back(listener);
	}

	void ProjectManager::RemoveListener(ProjectManagerListener* listener)
	{
		m_listeners.erase(linatl::find_if(m_listeners.begin(), m_listeners.end(), [listener](ProjectManagerListener* l) -> bool { return l == listener; }));
	}

	void ProjectManager::CreateEmptyProjectAndOpen(const String& path)
	{
		ProjectData dummy;
		dummy.SetPath(path);
		dummy.SetProjectName(FileSystem::GetFilenameOnlyFromPath(path));
		dummy.SaveToFile();

		const String resCache = dummy.GetResourceDirectory();
		if (FileSystem::FileOrPathExists(resCache))
			FileSystem::DeleteDirectory(resCache);

		OpenProject(path);
	}

	void ProjectManager::RemoveCurrentProject()
	{
		if (m_currentProject == nullptr)
			return;

		delete m_currentProject;
		m_currentProject = nullptr;

		for (ProjectManagerListener* listener : m_listeners)
			listener->OnProjectClosed();
	}

	void ProjectManager::OpenProject(const String& projectFile)
	{
		RemoveCurrentProject();
		m_currentProject = new ProjectData();
		m_currentProject->SetPath(projectFile);

		// Create resource directory if not existing.
		if (!FileSystem::FileOrPathExists(m_currentProject->GetResourceDirectory()))
			FileSystem::CreateFolderInPath(m_currentProject->GetResourceDirectory());

		// Lock windows & add progress.
		Widget* lock		 = m_editor->GetWindowPanelManager().LockAllForegrounds(m_primaryWidgetManager->GetRoot()->GetWindow(), Locale::GetStr(LocaleStr::WorkInProgressInAnotherWindow));
		Widget* progress	 = CommonWidgets::BuildGenericPopupProgress(m_primaryWidgetManager->GetRoot(), Locale::GetStr(LocaleStr::CreatingProject), true);
		Text*	progressText = static_cast<Text*>(progress->FindChildWithDebugName("Progress"));
		lock->AddChild(progress);

		Taskflow tf;
		tf.emplace([this, progressText]() {
			auto updateProg = [progressText](const String& txt) {
				progressText->GetProps().text = txt;
				progressText->CalculateTextSize();
			};

			updateProg(Locale::GetStr(LocaleStr::LoadingProjectData));
			m_currentProject->LoadFromFile();

			updateProg(Locale::GetStr(LocaleStr::VerifyingProjectResources));
			VerifyProjectResources(m_currentProject);

			updateProg(Locale::GetStr(LocaleStr::CreatingCoreResources));

			const Vector<ResourcePipeline::ResourceImportDef> desiredAssets = {
				{
					.path = EDITOR_FONT_ROBOTO_PATH,
					.id	  = EDITOR_FONT_ROBOTO_ID,
				},
				{
					.path = EDITOR_SHADER_DEFAULT_OBJECT_PATH,
					.id	  = EDITOR_SHADER_DEFAULT_OBJECT_ID,
				},
				{
					.path = EDITOR_SHADER_DEFAULT_SKY_PATH,
					.id	  = EDITOR_SHADER_DEFAULT_SKY_ID,
				},
				{
					.path = EDITOR_SHADER_DEFAULT_LIGHTING_PATH,
					.id	  = EDITOR_SHADER_DEFAULT_LIGHTING_ID,
				},
				{
					.path = EDITOR_MODEL_CUBE_PATH,
					.id	  = EDITOR_MODEL_CUBE_ID,
				},
				{
					.path = EDITOR_MODEL_SPHERE_PATH,
					.id	  = EDITOR_MODEL_SPHERE_ID,
				},
				{
					.path = EDITOR_MODEL_CYLINDER_PATH,
					.id	  = EDITOR_MODEL_CYLINDER_ID,
				},
				{
					.path = EDITOR_MODEL_PLANE_PATH,
					.id	  = EDITOR_MODEL_PLANE_ID,
				},
				{
					.path = EDITOR_MODEL_SKYCUBE_PATH,
					.id	  = EDITOR_MODEL_SKYCUBE_ID,
				},
				{
					.path = EDITOR_TEXTURE_EMPTY_ALBEDO_PATH,
					.id	  = EDITOR_TEXTURE_EMPTY_ALBEDO_ID,
				},
				{
					.path = EDITOR_TEXTURE_EMPTY_NORMAL_PATH,
					.id	  = EDITOR_TEXTURE_EMPTY_NORMAL_ID,
				},
			};

			Vector<ResourcePipeline::ResourceImportDef> importDefinitions;
			ResourceDirectory*							linaAssets = m_currentProject->GetResourceRoot().GetChildByName(EDITOR_DEF_RESOURCES_FOLDER);
			if (linaAssets == nullptr)
			{
				importDefinitions = desiredAssets;
				linaAssets		  = m_currentProject->GetResourceRoot().CreateChild({
						   .name	 = EDITOR_DEF_RESOURCES_FOLDER,
						   .isFolder = true,
				   });
			}
			else
			{
				for (const ResourcePipeline::ResourceImportDef& def : desiredAssets)
				{
					if (linaAssets->FindResource(def.id) == nullptr)
						importDefinitions.push_back(def);
				}
			}

			if (!importDefinitions.empty())
			{
				ResourcePipeline::ImportResources(m_currentProject, linaAssets, importDefinitions, [updateProg](uint32 count, const ResourcePipeline::ResourceImportDef& currentDef, bool isComplete) {
					if (!currentDef.path.empty())
						updateProg(currentDef.path);
				});
			}

			// Custom sampler.
			if (linaAssets->FindResource(EDITOR_SAMPLER_DEFAULT_ID) == nullptr)
			{
				updateProg(EDITOR_SAMPLER_DEFAULT_PATH);
				ResourcePipeline::SaveNewResource(m_currentProject, linaAssets, EDITOR_SAMPLER_DEFAULT_PATH, GetTypeID<TextureSampler>(), EDITOR_SAMPLER_DEFAULT_ID);
			}

			// updateProg(Locale::GetStr(LocaleStr::GeneratingThumbnails));
			// GenerateMissingAtlasImages(&m_currentProject->GetResourceRoot());

			updateProg(Locale::GetStr(LocaleStr::Saving));
			m_currentProject->SaveToFile();

			EditorSettings& settings = m_editor->GetSettings();
			settings.SetLastProjectPath(m_currentProject->GetPath());
			settings.SaveToFile();

			TaskRunner::QueueTask([this]() {
				m_editor->GetWindowPanelManager().UnlockAllForegrounds();
				Application::GetLGX()->Join();
				m_editor->GetAtlasManager().RefreshPoolAtlases();
				for (ProjectManagerListener* listener : m_listeners)
					listener->OnProjectOpened(m_currentProject);
			});
		});

		m_executor.RunMove(tf);
	}

	void ProjectManager::VerifyProjectResources(ProjectData* projectData)
	{
		// Check if the actual resource file for a ResourceDirectory in the project exists, if not delete the directory.
		ResourceDirectory* root = &projectData->GetResourceRoot();
		VerifyResourceDirectory(projectData, root);

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

		m_editor->GetProjectManager().SaveProjectChanges();
	}

	void ProjectManager::VerifyResourceDirectory(ProjectData* projectData, ResourceDirectory* dir)
	{
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
				VerifyResourceDirectory(projectData, c);
		}

		for (ResourceDirectory* d : killList)
			dir->DestroyChild(d);
	}

	void ProjectManager::NotifyProjectResourcesRefreshed()
	{
		for (ProjectManagerListener* listener : m_listeners)
			listener->OnProjectResourcesRefreshed();
	}

	TextureAtlasImage* ProjectManager::GetThumbnail(ResourceDirectory* dir)
	{
		return m_resourceThumbnails[dir->resourceID];
	}

	void ProjectManager::InvalidateThumbnail(ResourceDirectory* dir)
	{
		m_resourceThumbnails[dir->resourceID] = nullptr;
	}

	void ProjectManager::GenerateMissingAtlases(ResourceDirectory* dir)
	{
		for (ResourceDirectory* c : dir->children)
			GenerateMissingAtlases(c);

		if (dir->isFolder)
			return;

		auto& thumb = m_resourceThumbnailsOnFlight[dir->resourceID];

		if (thumb == nullptr)
		{

			if (dir->resourceTID == GetTypeID<Shader>())
				thumb = m_editor->GetAtlasManager().GetImageFromAtlas("ProjectIcons"_hs, "ShaderSmall"_hs);
			else if (dir->resourceTID == GetTypeID<EntityWorld>())
				thumb = m_editor->GetAtlasManager().GetImageFromAtlas("ProjectIcons"_hs, "WorldSmall"_hs);
			else if (dir->resourceTID == GetTypeID<PhysicsMaterial>())
				thumb = m_editor->GetAtlasManager().GetImageFromAtlas("ProjectIcons"_hs, "PhyMatSmall"_hs);
			else if (dir->resourceTID == GetTypeID<TextureSampler>())
				thumb = m_editor->GetAtlasManager().GetImageFromAtlas("ProjectIcons"_hs, "SamplerSmall"_hs);
			else if (dir->resourceTID == GetTypeID<GUIWidget>())
				thumb = m_editor->GetAtlasManager().GetImageFromAtlas("ProjectIcons"_hs, "WidgetSmall"_hs);
			else
			{
				if (dir->thumbnailBuffer.IsEmpty())
				{
					LinaGX::TextureBuffer buf = {};
					IStream				  stream;
					stream.Create(dir->thumbnailBuffer.GetRaw(), dir->thumbnailBuffer.GetSize());
					stream >> buf.width >> buf.height >> buf.bytesPerPixel;
					const size_t sz = static_cast<size_t>(buf.width * buf.height * buf.bytesPerPixel);
					buf.pixels		= new uint8[sz];
					stream.ReadToRaw(buf.pixels, sz);

					thumb = m_editor->GetAtlasManager().AddImageToAtlas(buf.pixels, Vector2ui(buf.width, buf.height), buf.bytesPerPixel == 4 ? LinaGX::Format::R8G8B8A8_SRGB : LinaGX::Format::R8_UNORM);

					ThumbnailGenerator::CreateThumbnailBuffer(dir->thumbnailBuffer, buf);
					delete[] buf.pixels;
					stream.Destroy();
				}
				else
				{
					LinaGX::TextureBuffer buf = ThumbnailGenerator::GenerateThumbnailForResource(dir->relativePathToProject, dir->resourceTID);
					ThumbnailGenerator::CreateThumbnailBuffer(dir->thumbnailBuffer, buf);

					thumb = m_editor->GetAtlasManager().AddImageToAtlas(buf.pixels, Vector2ui(buf.width, buf.height), buf.bytesPerPixel == 4 ? LinaGX::Format::R8G8B8A8_SRGB : LinaGX::Format::R8_UNORM);

					delete[] buf.pixels;
				}
			}
		}
	}
} // namespace Lina::Editor
