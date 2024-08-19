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

#include "Editor/IO/DirectoryItem.hpp"
#include "Common/FileSystem/FileSystem.hpp"

namespace Lina::Editor
{
	bool DirectoryItem::ContainsSearchString(const String& str, bool recursive, bool onlyDirectories)
	{
		if (UtilStr::ToLower(name).find(UtilStr::ToLower(str)) != String::npos)
			return true;

		if (!recursive)
			return false;

		for (DirectoryItem* c : children)
		{
			if (onlyDirectories && !c->isDirectory)
				continue;

			if (c->ContainsSearchString(str, recursive))
				return true;
		}

		return false;
	}
	String DirectoryItem::GetNewItemName(const String& originalName)
	{
		auto it = linatl::find_if(children.begin(), children.end(), [originalName](DirectoryItem* c) -> bool { return c->name.compare(originalName) == 0; });

		if (it != children.end())
			return GetNewItemName(originalName + " (Copy)");

		return originalName;
	}
	DirectoryItem* DirectoryItem::GetChildrenByName(const String& name)
	{
		auto it = linatl::find_if(children.begin(), children.end(), [name](DirectoryItem* c) -> bool { return c->name.compare(name) == 0; });
		if (it != children.end())
			return *it;

		return nullptr;
	}

	DirectoryItem* DirectoryItem::GetChildrenByRelativePath(const String& relativePath, bool isRecursive)
	{
		auto it = linatl::find_if(children.begin(), children.end(), [relativePath](DirectoryItem* c) -> bool { return c->relativePath.compare(relativePath) == 0; });
		if (it != children.end())
			return *it;

		if (isRecursive)
		{
			for (DirectoryItem* c : children)
			{
				DirectoryItem* found = c->GetChildrenByRelativePath(relativePath, isRecursive);
				if (found)
					return found;
			}
		}

		return nullptr;
	}
	DirectoryItem* DirectoryItem::GetChildrenByAbsPath(const String& absPath, bool isRecursive)
	{
		auto it = linatl::find_if(children.begin(), children.end(), [absPath](DirectoryItem* c) -> bool { return c->absolutePath.compare(absPath) == 0; });
		if (it != children.end())
			return *it;

		if (isRecursive)
		{
			for (DirectoryItem* c : children)
			{
				DirectoryItem* found = c->GetChildrenByAbsPath(absPath, isRecursive);
				if (found)
					return found;
			}
		}

		return nullptr;
	}

	void DirectoryItem::GetItemsContainingSearchString(Vector<DirectoryItem*>& outItems, const String& str, bool isRecursive)
	{
		for (DirectoryItem* c : children)
		{
			if (c->name.find(str) != String::npos)
				outItems.push_back(c);

			if (isRecursive)
				c->GetItemsContainingSearchString(outItems, str, isRecursive);
		}
	}

	void DirectoryItem::OnParentPathChanged()
	{
		const String& parentAbs = parent->absolutePath;
		const String& parentRel = parent->relativePath;

		absolutePath = parentAbs + "/" + name;
		relativePath = parentRel + "/" + name;
		FileSystem::FixPath(absolutePath);
		FileSystem::FixPath(relativePath);

		for (DirectoryItem* child : children)
			child->OnParentPathChanged();
	}
	void DirectoryItem::Rename(const String& newName)
	{
		const String oldPath = absolutePath;
		absolutePath		 = FileSystem::GetFilePath(absolutePath) + newName;
		relativePath		 = FileSystem::GetFilePath(relativePath) + newName;
		name				 = newName;
		sid					 = TO_SID(relativePath);
		FileSystem::ChangeDirectoryName(oldPath, absolutePath);

		for (DirectoryItem* child : children)
			child->OnParentPathChanged();
	}
} // namespace Lina::Editor
