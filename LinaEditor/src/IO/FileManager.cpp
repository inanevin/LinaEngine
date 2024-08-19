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

#include "Editor/IO/FileManager.hpp"
#include "Editor/IO/ExtensionSupport.hpp"
#include "Editor/Editor.hpp"
#include "Editor/IO/ThumbnailGenerator.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Common/System/System.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/FileSystem/FileWatcher.hpp"
#include "Common/System/System.hpp"
#include "Common/Math/Math.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/GfxManager.hpp"

namespace Lina::Editor
{

	String FileManager::GetMetacachePath()
	{
		return FileSystem::GetUserDataFolder() + "Editor/ResourceCache/";
	}

	void FileManager::Initialize(Editor* editor)
	{
		m_editor = editor;
		m_editor->GetProjectManager().AddListener(this);
	}

	void FileManager::PreTick()
	{
		for (Vector<ThumbnailGenerator*>::iterator it = m_thumbnailGenerators.begin(); it != m_thumbnailGenerators.end();)
		{
			ThumbnailGenerator* generator = *it;
			if (generator->GetStatus() == ThumbnailGenerator::Status::Done)
			{
				for (FileManagerListener* l : m_listeners)
					l->OnFileManagerThumbnailsGenerated(generator->GetRootItem(), generator->GetIsRecursive());

				delete generator;
				it = m_thumbnailGenerators.erase(it);
			}
			else
				++it;
		}
	}

	void FileManager::Shutdown()
	{
		m_editor->GetProjectManager().RemoveListener(this);

		for (ThumbnailGenerator* gen : m_thumbnailGenerators)
			delete gen;
		m_thumbnailGenerators.clear();
		ClearResources();
	}

	void FileManager::ClearResources()
	{
		if (m_root != nullptr)
			DeallocItem(m_root);
		m_root = nullptr;
	}

	void FileManager::DeallocItem(DirectoryItem* item)
	{
		for (auto* i : item->children)
			DeallocItem(i);

		if (item->textureAtlas != nullptr)
		{
			m_editor->GetAtlasManager().RemoveImage(item->textureAtlas);
			item->textureAtlas = nullptr;
		}

		item->children.clear();
		m_itemBucket.Free(item);
	}

	void FileManager::ScanItem(DirectoryItem* item)
	{
		if (!item->isDirectory)
			return;

		// Remove children that don't exist.
		for (Vector<DirectoryItem*>::iterator it = item->children.begin(); it != item->children.end();)
		{
			DirectoryItem* child = *it;
			if (!FileSystem::FileOrPathExists(child->absolutePath))
			{
				it = item->children.erase(it);
				DeallocItem(child);
			}
			else
				it++;
		}

		Vector<String> dirs = {};

		FileSystem::GetFilesAndFoldersInDirectory(item->absolutePath, dirs);

		for (const auto& str : dirs)
		{
			auto it = linatl::find_if(item->children.begin(), item->children.end(), [str](DirectoryItem* it) -> bool { return it->absolutePath.compare(str) == 0; });
			if (it != item->children.end())
				continue;

			const bool isDirectory = FileSystem::IsDirectory(str);
			String	   extension   = "";
			TypeID	   tid		   = 0;

			if (!isDirectory)
			{
				extension = FileSystem::GetFileExtension(str);
				tid		  = ExtensionSupport::GetTypeIDForExtension(extension);
				if (tid == 0)
					continue;
			}

			DirectoryItem* subItem = m_itemBucket.Allocate();
			subItem->extension	   = extension;
			subItem->tid		   = tid;
			subItem->isDirectory   = isDirectory;
			subItem->parent		   = item;
			subItem->outlineFX	   = !subItem->isDirectory && ExtensionSupport::RequiresOutlineFX(tid);

			item->children.push_back(subItem);

			FillPathInformation(subItem, str);

			if (subItem->isDirectory)
				ScanItem(subItem);
		}

		// Sort alphabetically

		Vector<DirectoryItem*> folders = {}, files = {};
		for (auto* c : item->children)
		{
			if (c->isDirectory)
				folders.push_back(c);
			else
				files.push_back(c);
		}
		linatl::sort(folders.begin(), folders.end(), [](const DirectoryItem* a, const DirectoryItem* b) { return UtilStr::ToLower(a->name) < UtilStr::ToLower(b->name); });
		linatl::sort(files.begin(), files.end(), [](const DirectoryItem* a, const DirectoryItem* b) { return UtilStr::ToLower(a->name) < UtilStr::ToLower(b->name); });

		item->children.clear();
		item->children.insert(item->children.end(), folders.begin(), folders.end());
		item->children.insert(item->children.end(), files.begin(), files.end());

		DirectoryItem* prev = nullptr;
		for (DirectoryItem* child : item->children)
		{
			if (prev != nullptr)
			{
				child->prev = prev;
				prev->next	= child;
			}
			prev = child;
		}
	}

	void FileManager::ScanItemRecursively(DirectoryItem* item)
	{
		ScanItem(item);

		for (DirectoryItem* c : item->children)
			ScanItemRecursively(c);
	}

	void FileManager::GenerateThumbnails(DirectoryItem* root, bool recursive)
	{
		m_thumbnailGenerators.push_back(new ThumbnailGenerator(m_editor, &m_executor, root, recursive));
	}

	void FileManager::OnProjectOpened(ProjectData* data)
	{
		SetProjectDirectory(FileSystem::GetFilePath(data->GetPath()));
		RefreshResources();
	}

	void FileManager::RefreshResources()
	{
		ClearResources();

		const String resDir = m_projectDirectory + ""
												   "Resources/";
		if (!FileSystem::FileOrPathExists(resDir))
			return;

		m_root				= m_itemBucket.Allocate();
		m_root->isDirectory = true;
		FillPathInformation(m_root, resDir);
		ScanItem(m_root);

		m_thumbnailGenerators.push_back(new ThumbnailGenerator(m_editor, &m_executor, m_root, true));
	}

	void FileManager::ClearDirectory(DirectoryItem* item)
	{
		for (auto* c : item->children)
			DeallocItem(c);

		item->children.clear();
	}

	void FileManager::RefreshDirectory(DirectoryItem* item)
	{
		ClearDirectory(item);
		ScanItem(item);
	}

	void FileManager::FillPathInformation(DirectoryItem* item, const String& fullAbsPath)
	{
		const String basePath = FileSystem::GetFilePath(m_editor->GetProjectManager().GetProjectData()->GetPath());
		const size_t baseSz	  = basePath.size();

		item->absolutePath = FileSystem::FixPath(fullAbsPath);
		item->relativePath = FileSystem::FixPath(fullAbsPath.substr(baseSz, fullAbsPath.size()));
		if (item->isDirectory)
			item->name = FileSystem::GetLastFolderFromPath(fullAbsPath);
		else
			item->name = FileSystem::GetFilenameAndExtensionFromPath(item->absolutePath);

		item->lastModifiedDate = TO_SID(FileSystem::GetLastModifiedDate(item->absolutePath));
		item->sid			   = TO_SID(item->relativePath);
	}

	DirectoryItem* FileManager::FindItemFromRelativePath(const String& relativePath, DirectoryItem* searchRoot)
	{
		if (searchRoot == nullptr)
			searchRoot = m_root;

		if (searchRoot->relativePath.compare(relativePath) == 0)
			return searchRoot;

		for (auto* c : searchRoot->children)
		{
			DirectoryItem* item = FindItemFromRelativePath(relativePath, c);
			if (item)
				return item;
		}

		return nullptr;
	}

	void FileManager::AddListener(FileManagerListener* listener)
	{
		m_listeners.push_back(listener);
	}

	void FileManager::RemoveListener(FileManagerListener* listener)
	{
		m_listeners.erase(linatl::find_if(m_listeners.begin(), m_listeners.end(), [listener](FileManagerListener* l) -> bool { return l == listener; }));
	}

} // namespace Lina::Editor
