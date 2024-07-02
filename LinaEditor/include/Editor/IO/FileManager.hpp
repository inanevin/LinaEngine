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
#include "Common/Memory/AllocatorBucket.hpp"
#include "ThumbnailGenerator.hpp"
#include "DirectoryItem.hpp"

namespace Lina::Editor
{
	class Editor;

	class FileManager
	{
	public:
		FileManager()  = default;
		~FileManager() = default;

		void		   Initialize(Editor* editor);
		void		   Shutdown();
		void		   PreTick();
		void		   RefreshResources();
		void		   ClearResources();
		DirectoryItem* FindItemFromRelativePath(const String& relativePath, DirectoryItem* searchRoot);
		void		   GetMetacachePath(String& outPath, const String& filePath) const;

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
		void FillPathInformation(DirectoryItem* item, const String& fullAbsPath);

	private:
		ThumbnailGenerator					m_thumbnailGenerator;
		String								m_projectDirectory = "";
		Editor*								m_editor		   = nullptr;
		AllocatorBucket<DirectoryItem, 200> m_itemBucket;
		DirectoryItem*						m_root = nullptr;
	};

} // namespace Lina::Editor
