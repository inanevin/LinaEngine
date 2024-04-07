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
#include "Editor/Editor.hpp"
#include "Common/System/System.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/FileSystem/FileWatcher.hpp"

namespace Lina::Editor
{

	void FileManager::Initialize(Editor* editor)
	{
		m_editor = editor;
	}

	void FileManager::Shutdown()
	{
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

		item->children.clear();
		m_allocatorPool.Free(item);
	}

	void FileManager::ScanItem(DirectoryItem* item)
	{
		LINA_ASSERT(item->children.empty(), "");
		Vector<String> dirs = {};
		FileSystem::GetFilesAndFoldersInDirectory(item->path, dirs);

		for (const auto& str : dirs)
		{
			const bool isDirectory = FileSystem::IsDirectory(str);
			String	   extension   = "";
			TypeID	   tid		   = 0;

			if (!isDirectory)
			{
				extension = FileSystem::GetFileExtension(str);

				const auto& resourceCaches = m_editor->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager)->GetCaches();

				for (const auto& [typeID, cache] : resourceCaches)
				{
					if (cache->DoesSupportExtension(extension))
					{
						tid = typeID;
						break;
					}
				}

				if (tid == 0)
					continue;
			}

			DirectoryItem* subItem = static_cast<DirectoryItem*>(m_allocatorPool.Allocate(sizeof(DirectoryItem)));
			subItem->extension	   = extension;
			subItem->tid		   = tid;
			subItem->path		   = str;
			subItem->isDirectory   = isDirectory;
			item->children.push_back(subItem);

			if (subItem->isDirectory)
				ScanItem(subItem);
		}
	}

	void FileManager::RefreshResources()
	{
		ClearResources();

		const String resDir = m_projectDirectory + "Resources/";
		if (!FileSystem::FileOrPathExists(resDir))
			return;

		m_root				= static_cast<DirectoryItem*>(m_allocatorPool.Allocate(sizeof(DirectoryItem)));
		m_root->path		= resDir;
		m_root->isDirectory = true;
		ScanItem(m_root);
	}
} // namespace Lina::Editor
