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

	void FileManager::Initialize(Editor* editor)
	{
		m_editor = editor;
		m_thumbnailGenerator.Initialize(editor);
	}

	void FileManager::PreTick()
	{
		m_thumbnailGenerator.PreTick();
	}

	void FileManager::Shutdown()
	{
		m_thumbnailGenerator.Shutdown();
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
			auto* rm = m_editor->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
			m_editor->GetAtlasManager().RemoveImage(item->textureAtlas);
			item->textureAtlas = nullptr;
		}

		item->children.clear();
		m_itemBucket.Free(item);
	}

	void FileManager::ScanItem(DirectoryItem* item)
	{
		LINA_ASSERT(item->children.size() == 0, "");
		Vector<String> dirs = {};
		FileSystem::GetFilesAndFoldersInDirectory(item->absolutePath, dirs);

		for (const auto& str : dirs)
		{
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
		linatl::sort(folders.begin(), folders.end(), [](const DirectoryItem* a, const DirectoryItem* b) { return a->folderName < b->folderName; });
		linatl::sort(files.begin(), files.end(), [](const DirectoryItem* a, const DirectoryItem* b) { return a->fileName < b->fileName; });

		item->children.clear();
		item->children.insert(item->children.end(), folders.begin(), folders.end());
		item->children.insert(item->children.end(), files.begin(), files.end());
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

		m_thumbnailGenerator.GenerateThumbnail(m_root, true);
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

	void FileManager::UpdateItem(DirectoryItem* item, const String& newPath, bool regenerateThumbnail)
	{
		item->absolutePath = newPath;
	}

	void FileManager::FillPathInformation(DirectoryItem* item, const String& fullAbsPath)
	{
		const String basePath = FileSystem::GetFilePath(m_editor->GetProjectManager().GetProjectData()->GetPath());
		const size_t baseSz	  = basePath.size();

		item->absolutePath = fullAbsPath;
		item->relativePath = fullAbsPath.substr(baseSz, fullAbsPath.size());
		if (item->isDirectory)
			item->folderName = FileSystem::GetLastFolderFromPath(fullAbsPath);
		else
			item->fileName = FileSystem::GetFilenameAndExtensionFromPath(item->absolutePath);

		item->sid = TO_SID(item->relativePath);
	}

	DirectoryItem* FileManager::FindItemFromRelativePath(const String& relativePath, DirectoryItem* searchRoot)
	{
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

	void FileManager::GetMetacachePath(String& outPath, const String& filePath) const
	{
		const StringID sid = TO_SID(filePath);
		outPath.clear();
		outPath = "";

		ProjectData* pd = m_editor->GetProjectManager().GetProjectData();

		if (pd == nullptr)
			outPath = FileSystem::GetRunningDirectory() + "/Cache/";
		else
			outPath = pd->GetPath() + "/Cache/";

		outPath += TO_STRING(sid) + ".linameta";
	}
} // namespace Lina::Editor
