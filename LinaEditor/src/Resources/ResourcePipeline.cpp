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
#include "Core/Meta/ProjectData.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/World/EntityWorld.hpp"

namespace Lina::Editor
{
	void ResourcePipeline::Initialize(Editor* editor)
	{
		m_editor = editor;

		if (!FileSystem::FileOrPathExists(GetResourceDirectory()))
			FileSystem::CreateFolderInPath(GetResourceDirectory());
	}

	ResourceID ResourcePipeline::SaveNewResource(TypeID tid)
	{
		const ResourceID id = m_editor->GetProjectManager().GetProjectData()->ConsumeResourceID();
		m_editor->GetProjectManager().SaveProjectChanges();

		if (tid == GetTypeID<GUIWidget>())
		{
			GUIWidget w(id);
			w.SaveToFileAsBinary(GetResourceDirectory() + "GUIWidget_" + TO_STRING(id) + ".linawidget");
		}
		else if (tid == GetTypeID<Material>())
		{
			Material mat(id);
			mat.SaveToFileAsBinary(GetResourceDirectory() + "Material_" + TO_STRING(id) + ".linamaterial");
		}

		return id;
	}

	String ResourcePipeline::GetResourceDirectory()
	{
		const String project = m_editor->GetProjectManager().GetProjectData()->GetPath();
		return FileSystem::GetFilePath(project) + "_LinaResourceCache/";
	}
} // namespace Lina::Editor
