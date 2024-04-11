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

#pragma once

#include "Common/Data/String.hpp"
#include "Common/Memory/MemoryAllocatorPool.hpp"

namespace Lina
{
	class Texture;
}
namespace Lina::Editor
{

	class Editor;

	struct DirectoryItem
	{
		bool				   isDirectory	= false;
		String				   absolutePath = "";
		String				   relativePath = "";
		String				   folderName	= "";
		String				   fileName		= "";
		StringID			   sid			= 0;
		TypeID				   tid			= 0;
		String				   extension	= "";
		Vector<DirectoryItem*> children		= {};
		Texture*			   thumbnail	= nullptr;
		DirectoryItem*		   parent		= nullptr;
	};

	class FileManager
	{
	public:
		FileManager() : m_allocatorPool(AllocatorType::Pool, AllocatorGrowPolicy::UseInitialSize, false, sizeof(DirectoryItem) * 100, sizeof(DirectoryItem)){};

		void Initialize(Editor* editor);
		void Shutdown();
		void RefreshResources();
		void ClearResources();

		inline void SetProjectDirectory(const String& dir)
		{
			m_projectDirectory = dir;
		}

		inline DirectoryItem* GetRoot() const
		{
			return m_root;
		}

	private:
		void ScanItem(DirectoryItem* item);
		void DeallocItem(DirectoryItem* item);
		void ClearDirectory(DirectoryItem* item);
		void RefreshDirectory(DirectoryItem* item);
		void UpdateItem(DirectoryItem* item, const String& newPath, bool regenerateThumbnail);
		void GenerateThumbnailForItem(DirectoryItem* item);
		void FillPathInformation(DirectoryItem* item, const String& fullAbsPath);

		void GenerateThumbTexture(DirectoryItem* item);
		void GenerateThumbFont(DirectoryItem* item);

	private:
		String				m_projectDirectory = "";
		Editor*				m_editor		   = nullptr;
		MemoryAllocatorPool m_allocatorPool;
		DirectoryItem*		m_root = nullptr;
	};

} // namespace Lina::Editor
