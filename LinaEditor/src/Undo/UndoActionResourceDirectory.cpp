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

#include "Editor/Undo/UndoActionResourceDirectory.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Widgets/Panel/PanelResourceBrowser.hpp"
#include "Editor/Widgets/Compound/ResourceDirectoryBrowser.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Common/Serialization/Serialization.hpp"

namespace Lina::Editor
{

	UndoActionResourceRename* UndoActionResourceRename::Create(Editor* editor, ResourceID resourceID, const String& newName)
	{
		UndoActionResourceRename* action = new UndoActionResourceRename();
		ResourceDirectory*		  dir	 = editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindResourceDirectory(resourceID);
		action->m_prevName				 = dir->name;
		action->m_newName				 = newName;
		action->m_resourceID			 = resourceID;
		editor->GetUndoManager().AddToStack(action);
		return action;
	}

	void UndoActionResourceRename::Execute(Editor* editor)
	{
		ResourceDirectory* dir = editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindResourceDirectory(m_resourceID);
		if (dir == nullptr)
			return;

		dir->name = m_newName;

		// Open, modify, close resource.
		MetaType&	 meta	 = ReflectionSystem::Get().Resolve(dir->resourceTID);
		Resource*	 res	 = static_cast<Resource*>(meta.GetFunction<void*()>("Allocate"_hs)());
		const String resPath = editor->GetProjectManager().GetProjectData()->GetResourcePath(dir->resourceID);
		IStream		 stream	 = Serialization::LoadFromFile(resPath.c_str());
		if (!stream.Empty())
		{
			res->LoadFromStream(stream);
			res->SetName(dir->name);
			res->SaveToFileAsBinary(resPath);
		}
		stream.Destroy();
		meta.GetFunction<void(void*)>("Deallocate"_hs)(res);

		editor->GetProjectManager().SaveProjectChanges();

		Panel* panel = editor->GetWindowPanelManager().FindPanelOfType(PanelType::ResourceBrowser, 0);
		if (panel != nullptr)
		{
			PanelResourceBrowser* browser = static_cast<PanelResourceBrowser*>(panel);
			browser->GetBrowser()->RefreshDirectory();
		}
	}

	void UndoActionResourceRename::Undo(Editor* editor)
	{
		ResourceDirectory* dir = editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindResourceDirectory(m_resourceID);
		if (dir == nullptr)
			return;

		dir->name = m_prevName;

		// Open, modify, close resource.
		MetaType&	 meta	 = ReflectionSystem::Get().Resolve(dir->resourceTID);
		Resource*	 res	 = static_cast<Resource*>(meta.GetFunction<void*()>("Allocate"_hs)());
		const String resPath = editor->GetProjectManager().GetProjectData()->GetResourcePath(dir->resourceID);
		IStream		 stream	 = Serialization::LoadFromFile(resPath.c_str());
		if (!stream.Empty())
		{
			res->LoadFromStream(stream);
			res->SetName(dir->name);
			res->SaveToFileAsBinary(resPath);
		}
		stream.Destroy();
		meta.GetFunction<void(void*)>("Deallocate"_hs)(res);

		editor->GetProjectManager().SaveProjectChanges();

		Panel* panel = editor->GetWindowPanelManager().FindPanelOfType(PanelType::ResourceBrowser, 0);
		if (panel != nullptr)
		{
			PanelResourceBrowser* browser = static_cast<PanelResourceBrowser*>(panel);
			browser->GetBrowser()->RefreshDirectory();
		}
	}
} // namespace Lina::Editor
