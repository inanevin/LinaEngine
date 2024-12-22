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

#include "Editor/Actions/EditorActionResources.hpp"
#include "Editor/Editor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Widgets/Panel/PanelResourceBrowser.hpp"
#include "Editor/Widgets/Panel/PanelFontViewer.hpp"
#include "Editor/Widgets/Panel/PanelMaterialViewer.hpp"
#include "Editor/Widgets/Panel/PanelModelViewer.hpp"
#include "Editor/Widgets/Panel/PanelTextureViewer.hpp"
#include "Editor/Widgets/Panel/PanelPhysicsMaterialViewer.hpp"
#include "Editor/Widgets/Panel/PanelAudioViewer.hpp"
#include "Editor/Widgets/Panel/PanelSamplerViewer.hpp"
#include "Editor/Widgets/Panel/PanelResourceBrowser.hpp"
#include "Editor/Widgets/Compound/ResourceDirectoryBrowser.hpp"
#include "Editor/IO/ThumbnailGenerator.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Core/Application.hpp"
#include "Editor/Resources/ResourcePipeline.hpp"

namespace Lina::Editor
{

	/*
	*** RESOURCE RENAME
	*/

	EditorActionResourceRename* EditorActionResourceRename::Create(Editor* editor, GUID guid, const String& oldName, const String& newName)
	{
		EditorActionResourceRename* action = new EditorActionResourceRename();
		action->m_prevName				   = oldName;
		action->m_newName				   = newName;
		action->m_resourceGUID			   = guid;
		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}

	void EditorActionResourceRename::Execute(Editor* editor, ExecType type)
	{
		ResourceDirectory* dir = editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindByGUID(m_resourceGUID);
		if (dir == nullptr)
			return;

		if (type == ExecType::Create || type == ExecType::Redo)
			dir->name = m_newName;
		else
			dir->name = m_prevName;

		editor->GetProjectManager().SaveProjectChanges();

		Panel* panel = editor->GetWindowPanelManager().FindPanelOfType(PanelType::ResourceBrowser, 0);
		if (panel != nullptr)
		{
			PanelResourceBrowser* browser = static_cast<PanelResourceBrowser*>(panel);
			browser->GetBrowser()->RefreshDirectory();
		}

		if (dir->isFolder)
			return;

		const String resPath = editor->GetProjectManager().GetProjectData()->GetResourcePath(dir->resourceID);

		Resource* existing = editor->GetApp()->GetResourceManager().GetIfExists(dir->resourceTID, dir->resourceID);
		if (existing)
		{
			existing->SetName(dir->name);
			existing->SaveToFileAsBinary(resPath);
		}
		else
		{
			// Open, modify, close resource.
			MetaType* meta	 = ReflectionSystem::Get().Resolve(dir->resourceTID);
			Resource* res	 = static_cast<Resource*>(meta->GetFunction<void*()>("Allocate"_hs)());
			IStream	  stream = Serialization::LoadFromFile(resPath.c_str());
			if (!stream.Empty())
			{
				res->LoadFromStream(stream);
				res->SetName(dir->name);
				res->SaveToFileAsBinary(resPath);
			}
			stream.Destroy();
			meta->GetFunction<void(void*)>("Deallocate"_hs)(res);
		}

		// Find and update any resource panel using this.
		Vector<PanelResourceViewer*> panels = editor->GetWindowPanelManager().FindResourceViewers(dir->resourceID);
		for (PanelResourceViewer* panel : panels)
		{
			panel->UpdateResourceProperties();
			panel->RebuildContents();
			panel->GetWidgetProps().debugName = dir->name;
			panel->RefreshTab();
		}
	}

	/*
	*** SAMPLER DATA CHANGED
	*/

	EditorActionResourceSampler* EditorActionResourceSampler::Create(Editor* editor, ResourceID resourceID, const LinaGX::SamplerDesc& prevDesc, const LinaGX::SamplerDesc& newDesc)
	{
		EditorActionResourceSampler* action = new EditorActionResourceSampler();
		action->m_resourceID				= resourceID;
		action->m_prevDesc					= prevDesc;
		action->m_newDesc					= newDesc;
		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}

	void EditorActionResourceSampler::Execute(Editor* editor, ExecType type)
	{
		Resource* res = editor->GetApp()->GetResourceManager().GetIfExists<TextureSampler>(m_resourceID);
		if (res == nullptr)
			return;

		Panel*				 panel		  = editor->GetWindowPanelManager().FindPanelOfType(PanelType::SamplerViewer, m_resourceID);
		PanelResourceViewer* panelResView = panel ? static_cast<PanelResourceViewer*>(panel) : nullptr;
		TextureSampler*		 sampler	  = static_cast<TextureSampler*>(res);

		if (type == ExecType::Redo)
		{
			sampler->GetDesc() = m_newDesc;
		}
		else if (type == ExecType::Undo)
		{
			sampler->GetDesc() = m_prevDesc;
		}

		if (panelResView)
		{
			panelResView->StoreEditorActionBuffer();
			panelResView->UpdateResourceProperties();
			panelResView->RebuildContents();
		}

		sampler->SaveToFileAsBinary(editor->GetProjectManager().GetProjectData()->GetResourcePath(sampler->GetID()));
		editor->GetApp()->GetResourceManager().ReloadResourceHW({sampler});
	}

	/*
	*** FONT DATA CHANGED
	*/

	EditorActionResourceFont* EditorActionResourceFont::Create(Editor* editor, ResourceID resourceID, const Font::Metadata& prevMeta, const Font::Metadata& newMeta)
	{
		EditorActionResourceFont* action = new EditorActionResourceFont();
		action->m_resourceID			 = resourceID;
		action->m_prevMeta				 = prevMeta;
		action->m_newMeta				 = newMeta;
		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}

	void EditorActionResourceFont::Execute(Editor* editor, ExecType type)
	{
		Resource* res = editor->GetApp()->GetResourceManager().GetIfExists<Font>(m_resourceID);
		if (res == nullptr)
			return;

		Panel*				 panel		  = editor->GetWindowPanelManager().FindPanelOfType(PanelType::FontViewer, m_resourceID);
		PanelResourceViewer* panelResView = panel ? static_cast<PanelResourceViewer*>(panel) : nullptr;
		Font*				 font		  = static_cast<Font*>(res);

		EditorTask* task   = editor->GetTaskManager().CreateTask();
		task->ownerWindow  = panel->GetWindow();
		task->title		   = Locale::GetStr(LocaleStr::Saving);
		task->progressText = Locale::GetStr(LocaleStr::Working);

		Font::Metadata targetMeta = type == ExecType::Undo ? m_prevMeta : m_newMeta;

		task->task = [res, editor, targetMeta, type]() {
			Font* font = static_cast<Font*>(res);
			if (type != ExecType::Create)
				font->GetMeta() = targetMeta;

			res->SaveToFileAsBinary(editor->GetProjectManager().GetProjectData()->GetResourcePath(res->GetID()));
		};

		task->onComplete = [res, editor, panelResView, type]() {
			editor->GetApp()->GetResourceManager().ReloadResourceHW({res});

			if (panelResView)
			{
				panelResView->StoreEditorActionBuffer();
				panelResView->UpdateResourceProperties();
				panelResView->RebuildContents();
			}
		};

		editor->GetTaskManager().AddTask(task);
	}

	/*
	** TEXTURE
	*/

	EditorActionResourceTexture* EditorActionResourceTexture::Create(Editor* editor, ResourceID resourceID, const Texture::Metadata& prevMeta, const Texture::Metadata& newMeta)
	{
		EditorActionResourceTexture* action = new EditorActionResourceTexture();
		action->m_resourceID				= resourceID;
		action->m_prevMeta					= prevMeta;
		action->m_newMeta					= newMeta;
		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}

	void EditorActionResourceTexture::Execute(Editor* editor, ExecType type)
	{
		Resource* res = editor->GetApp()->GetResourceManager().GetIfExists<Texture>(m_resourceID);
		if (res == nullptr)
			return;

		Panel*				 panel		  = editor->GetWindowPanelManager().FindPanelOfType(PanelType::TextureViewer, m_resourceID);
		PanelResourceViewer* panelResView = panel ? static_cast<PanelResourceViewer*>(panel) : nullptr;
		Texture*			 txt		  = editor->GetApp()->GetResourceManager().GetResource<Texture>(m_resourceID);

		EditorTask* task   = editor->GetTaskManager().CreateTask();
		task->ownerWindow  = panel->GetWindow();
		task->title		   = Locale::GetStr(LocaleStr::Saving);
		task->progressText = Locale::GetStr(LocaleStr::Working);

		Texture::Metadata targetMeta = type == ExecType::Undo ? m_prevMeta : m_newMeta;
		Texture::Metadata prevMeta	 = type == ExecType::Undo ? m_newMeta : m_prevMeta;

		task->task = [editor, txt, targetMeta, prevMeta, type]() {
			if (prevMeta.force8Bit != targetMeta.force8Bit)
			{
				ResourceDirectory* dir	= editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindResourceDirectory(txt->GetID());
				const String	   path = FileSystem::GetFilePath(editor->GetProjectManager().GetProjectData()->GetPath()) + dir->sourcePathRelativeToProject;

				if (type != ExecType::Create)
					txt->GetMeta() = targetMeta;

				if (FileSystem::FileOrPathExists(path))
					txt->LoadFromFile(path);
			}
			else
			{
				if (type != ExecType::Create)
					txt->GetMeta() = targetMeta;

				txt->VerifyMipmaps();
				txt->DetermineFormat();
			}

			txt->SaveToFileAsBinary(editor->GetProjectManager().GetProjectData()->GetResourcePath(txt->GetID()));
		};

		task->onComplete = [editor, res, panelResView]() {
			editor->GetApp()->GetResourceManager().ReloadResourceHW({res});

			if (panelResView)
			{
				panelResView->StoreEditorActionBuffer();
				panelResView->UpdateResourceProperties();
				panelResView->RebuildContents();
			}
		};

		editor->GetTaskManager().AddTask(task);
	}

	/*
	** MATERIAL
	*/

	EditorActionResourceMaterial* EditorActionResourceMaterial::Create(Editor* editor, ResourceID resourceID, uint64 resourceSpace, const OStream& prevStream, const OStream& newStream)
	{
		EditorActionResourceMaterial* action = new EditorActionResourceMaterial();
		action->m_resourceID				 = resourceID;
		action->m_resourceSpace				 = resourceSpace;
		action->m_prevStream.WriteRaw(prevStream.GetDataRaw(), prevStream.GetCurrentSize());
		action->m_newStream.WriteRaw(newStream.GetDataRaw(), newStream.GetCurrentSize());
		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}

	void EditorActionResourceMaterial::Execute(Editor* editor, ExecType type)
	{
		Resource* res = editor->GetApp()->GetResourceManager().GetIfExists<Material>(m_resourceID);
		if (res == nullptr)
			return;

		Panel*				 panel		  = editor->GetWindowPanelManager().FindPanelOfType(PanelType::MaterialViewer, m_resourceID);
		PanelResourceViewer* panelResView = panel ? static_cast<PanelMaterialViewer*>(panel) : nullptr;
		Material*			 mat		  = editor->GetApp()->GetResourceManager().GetResource<Material>(m_resourceID);

		if (type == ExecType::Undo)
		{
			IStream stream;
			stream.Create(m_prevStream.GetDataRaw(), m_prevStream.GetCurrentSize());
			mat->LoadFromStream(stream);
			stream.Destroy();
		}
		else if (type == ExecType::Redo)
		{
			IStream stream;
			stream.Create(m_newStream.GetDataRaw(), m_newStream.GetCurrentSize());
			mat->LoadFromStream(stream);
			stream.Destroy();
		}

		mat->SaveToFileAsBinary(editor->GetProjectManager().GetProjectData()->GetResourcePath(mat->GetID()));

		if (panelResView)
		{
			panelResView->StoreEditorActionBuffer();
			PanelMaterialViewer* matViewer = static_cast<PanelMaterialViewer*>(panelResView);

			if (type != ExecType::Create)
			{
				panelResView->UpdateResourceProperties();
				panelResView->RebuildContents();
			}

			matViewer->GetWorld()->LoadMissingResources(editor->GetApp()->GetResourceManager(), editor->GetProjectManager().GetProjectData(), {}, m_resourceSpace);
		}

		editor->GetApp()->GetResourceManager().ReloadResourceHW({mat});
	}

	EditorActionResourceMaterialShader* EditorActionResourceMaterialShader::Create(Editor* editor, ResourceID resourceID, uint64 resourceSpace, ResourceID prevShader, ResourceID newShader, const OStream& prevStream)
	{
		LINA_ASSERT(prevShader != newShader, "");
		EditorActionResourceMaterialShader* action = new EditorActionResourceMaterialShader();
		action->m_resourceID					   = resourceID;
		action->m_resourceSpace					   = resourceSpace;
		action->m_prevShader					   = prevShader;
		action->m_newShader						   = newShader;
		action->m_prevStream.WriteRaw(prevStream.GetDataRaw(), prevStream.GetCurrentSize());
		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}

	void EditorActionResourceMaterialShader::Execute(Editor* editor, ExecType type)
	{
		Resource* res = editor->GetApp()->GetResourceManager().GetIfExists<Material>(m_resourceID);
		if (res == nullptr)
			return;

		Panel*				 panel		  = editor->GetWindowPanelManager().FindPanelOfType(PanelType::MaterialViewer, m_resourceID);
		PanelResourceViewer* panelResView = panel ? static_cast<PanelMaterialViewer*>(panel) : nullptr;
		Material*			 mat		  = editor->GetApp()->GetResourceManager().GetResource<Material>(m_resourceID);

		const ResourceID targetShader = type == ExecType::Undo ? m_prevShader : m_newShader;

		Shader* shader = editor->GetApp()->GetResourceManager().GetIfExists<Shader>(m_resourceID);
		if (shader != nullptr)
		{
			mat->SetShader(shader);
		}
		else
		{
			const String path = editor->GetProjectManager().GetProjectData()->GetResourcePath(targetShader);

			if (!FileSystem::FileOrPathExists(path))
				return;

			IStream stream = Serialization::LoadFromFile(path.c_str());
			Shader	sh(0, "");
			sh.LoadFromStream(stream);
			mat->SetShader(&sh);
			stream.Destroy();
		}

		if (type == ExecType::Undo)
		{
			Material prevMat(0, "");
			IStream	 stream;
			stream.Create(m_prevStream.GetDataRaw(), m_prevStream.GetCurrentSize());
			prevMat.LoadFromStream(stream);
			stream.Destroy();
			mat->CopyPropertiesFrom(&prevMat);
		}
		else
		{
			const Vector<MaterialProperty*>& props = mat->GetProperties();
			for (MaterialProperty* p : props)
				ResourcePipeline::TrySetMaterialProperty(p);
		}

		if (panelResView)
		{
			PanelMaterialViewer* matViewer = static_cast<PanelMaterialViewer*>(panelResView);
			matViewer->StoreShaderID();
			matViewer->SetupWorld();
			panelResView->UpdateResourceProperties();
			panelResView->RebuildContents();
			matViewer->GetWorld()->LoadMissingResources(editor->GetApp()->GetResourceManager(), editor->GetProjectManager().GetProjectData(), {}, m_resourceSpace);
		}

		mat->SaveToFileAsBinary(editor->GetProjectManager().GetProjectData()->GetResourcePath(mat->GetID()));
		editor->GetApp()->GetResourceManager().ReloadResourceHW({mat});
	}

	EditorActionResourceMove* EditorActionResourceMove::Create(Editor* editor, const Vector<GUID>& resources, const Vector<GUID>& previousParents, GUID newParent)
	{
		EditorActionResourceMove* action = new EditorActionResourceMove();

		LINA_ASSERT(resources.size() == previousParents.size(), "");

		action->m_resourceGUIDs		  = resources;
		action->m_previousParentGUIDs = previousParents;
		action->m_newParentGUIDs	  = newParent;
		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}

	void EditorActionResourceMove::Execute(Editor* editor, ExecType type)
	{
		if (type == ExecType::Undo)
		{
			const size_t sz = m_resourceGUIDs.size();
			for (size_t i = 0; i < sz; i++)
			{
				GUID guid		= m_resourceGUIDs[i];
				GUID targetGUID = m_previousParentGUIDs[i];

				ResourceDirectory* dir = editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindByGUID(guid);
				if (dir == nullptr)
					continue;

				ResourceDirectory* target = editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindByGUID(targetGUID);
				if (target == nullptr)
					continue;

				if (dir->parent == target)
					continue;

				editor->GetProjectManager().GetProjectData()->MoveResourceDirectory(dir, target);
			}
		}
		else
		{
			ResourceDirectory* target = editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindByGUID(m_newParentGUIDs);
			if (target == nullptr)
				return;

			for (GUID guid : m_resourceGUIDs)
			{
				ResourceDirectory* dir = editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindByGUID(guid);
				if (dir == nullptr)
					continue;

				if (dir->parent == target)
					continue;

				editor->GetProjectManager().GetProjectData()->MoveResourceDirectory(dir, target);
			}
		}

		editor->GetProjectManager().SaveProjectChanges();
		editor->GetProjectManager().NotifyProjectResourcesRefreshed();
	}

	EditorActionResourceFav* EditorActionResourceFav::Create(Editor* editor, const Vector<GUID>& resources, bool isAdd)
	{
		EditorActionResourceFav* action = new EditorActionResourceFav();
		action->m_resourceGUIDs			= resources;
		action->m_isAdd					= isAdd;
		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}

	void EditorActionResourceFav::Execute(Editor* editor, ExecType type)
	{
		ProjectManager& projectManager = editor->GetProjectManager();

		for (GUID guid : m_resourceGUIDs)
		{
			ResourceDirectory* dir = projectManager.GetProjectData()->GetResourceRoot().FindByGUID(guid);
			if (dir == nullptr)
				continue;

			if (type == ExecType::Undo)
				dir->userData.isInFavourites = !m_isAdd;
			else
				dir->userData.isInFavourites = m_isAdd;
		}

		projectManager.SaveProjectChanges();

		Panel* panel = editor->GetWindowPanelManager().FindPanelOfType(PanelType::ResourceBrowser, 0);
		if (panel)
			static_cast<PanelResourceBrowser*>(panel)->GetBrowser()->RefreshDirectory();
	}

	EditorActionResourceModel* EditorActionResourceModel::Create(Editor* editor, ResourceID resourceID, uint64 resourceSpace, const OStream& prevStream, const OStream& newStream)
	{
		EditorActionResourceModel* action = new EditorActionResourceModel();
		action->m_resourceID			  = resourceID;
		action->m_resourceSpace			  = resourceSpace;
		action->m_prevStream.WriteRaw(prevStream.GetDataRaw(), prevStream.GetCurrentSize());
		action->m_newStream.WriteRaw(newStream.GetDataRaw(), newStream.GetCurrentSize());
		editor->GetEditorActionManager().AddToStack(action);
		return action;
	}

	void EditorActionResourceModel::Execute(Editor* editor, ExecType type)
	{
		Resource* res = editor->GetApp()->GetResourceManager().GetIfExists<Model>(m_resourceID);
		if (res == nullptr)
			return;

		Panel*				 panel		  = editor->GetWindowPanelManager().FindPanelOfType(PanelType::ModelViewer, m_resourceID);
		PanelResourceViewer* panelResView = panel ? static_cast<PanelModelViewer*>(panel) : nullptr;
		Model*				 model		  = editor->GetApp()->GetResourceManager().GetResource<Model>(m_resourceID);

		if (type == ExecType::Undo)
		{
			IStream stream;
			stream.Create(m_prevStream.GetDataRaw(), m_prevStream.GetCurrentSize());
			model->LoadFromStream(stream);
			stream.Destroy();
		}
		else if (type == ExecType::Redo)
		{
			IStream stream;
			stream.Create(m_newStream.GetDataRaw(), m_newStream.GetCurrentSize());
			model->LoadFromStream(stream);
			stream.Destroy();
		}

		model->SaveToFileAsBinary(editor->GetProjectManager().GetProjectData()->GetResourcePath(model->GetID()));

		if (panelResView)
		{
			panelResView->StoreEditorActionBuffer();
			PanelModelViewer* modelViewer = static_cast<PanelModelViewer*>(panelResView);

			if (type != ExecType::Create)
			{
				panelResView->UpdateResourceProperties();
				panelResView->RebuildContents();
			}

			modelViewer->GetWorld()->LoadMissingResources(editor->GetApp()->GetResourceManager(), editor->GetProjectManager().GetProjectData(), {}, m_resourceSpace);
			modelViewer->SetupWorld();
		}

		editor->GetApp()->GetResourceManager().ReloadResourceHW({model});
	}

} // namespace Lina::Editor
