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

#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Physics/PhysicsMaterial.hpp"
#include "Core/World/EntityWorld.hpp"

namespace Lina::Editor
{

	void ResourcePipeline::Initialize(Editor* editor)
	{
		m_editor = editor;

		if (!FileSystem::FileOrPathExists(GetResourceDirectory()))
			FileSystem::CreateFolderInPath(GetResourceDirectory());

		ResourceDirectory* root = &m_editor->GetProjectManager().GetProjectData()->GetResourceRoot();
		VerifyResources(root);
		m_editor->GetProjectManager().SaveProjectChanges();
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

	ResourceID ResourcePipeline::SaveNewResource(TypeID tid, uint32 subType)
	{
		const ResourceID id = m_editor->GetProjectManager().GetProjectData()->ConsumeResourceID();
		m_editor->GetProjectManager().SaveProjectChanges();

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
			w->LoadFromFile(path.c_str());
			ptr = w;
		}
		else if (tid == GetTypeID<Material>())
		{
			Material* m = new Material(0);
			m->LoadFromFile(path.c_str());
			ptr = m;
		}
		else if (tid == GetTypeID<EntityWorld>())
		{
			EntityWorld* w = new EntityWorld(0);
			w->LoadFromFile(path.c_str());
			ptr = w;
		}
		else if (tid == GetTypeID<Shader>())
		{
			Shader* s = new Shader(0);
			s->LoadFromFile(path.c_str());
			ptr = s;
		}
		else if (tid == GetTypeID<PhysicsMaterial>())
		{
			PhysicsMaterial* mat = new PhysicsMaterial(0);
			mat->LoadFromFile(path.c_str());
			ptr = mat;
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
} // namespace Lina::Editor
