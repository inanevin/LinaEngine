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

#include "Core/Resources/ResourceDirectory.hpp"

namespace Lina
{

	ResourceDirectory::~ResourceDirectory()
	{
		thumbnailBuffer.Destroy();

		for (ResourceDirectory* c : children)
			delete c;
	}

	void ResourceDirectory::SaveToStream(OStream& stream)
	{
		stream << VERSION;
		stream << relativePathToProject;
		stream << name;
		stream << isFolder;
		stream << resourceID;
		stream << resourceTID;
		stream << children;
		stream << thumbnailBuffer;
	}

	void ResourceDirectory::LoadFromStream(IStream& stream)
	{
		uint32 ver = 0;
		stream >> ver;
		stream >> relativePathToProject;
		stream >> name;
		stream >> isFolder;
		stream >> resourceID;
		stream >> resourceTID;
		stream >> children;
		stream >> thumbnailBuffer;
		for (ResourceDirectory* c : children)
			c->parent = this;
		SortChildren();
	}

	ResourceDirectory* ResourceDirectory::CreateChild(const ResourceDirectory& desc)
	{
		ResourceDirectory* child = new ResourceDirectory();
		*child					 = desc;
		AddChild(child);
		return child;
	}

	void ResourceDirectory::AddChild(ResourceDirectory* dir)
	{
		children.push_back(dir);
		dir->parent = this;
		SortChildren();
	}

	void ResourceDirectory::RemoveChild(ResourceDirectory* dir)
	{
		auto it = linatl::find_if(children.begin(), children.end(), [dir](ResourceDirectory* c) -> bool { return c == dir; });
		children.erase(it);
		SortChildren();
	}

	void ResourceDirectory::DestroyChild(ResourceDirectory* dir)
	{
		RemoveChild(dir);
		delete dir;
	}

	void ResourceDirectory::SortChildren()
	{
		Vector<ResourceDirectory*> folders, files;
		for (ResourceDirectory* c : children)
		{
			if (c->isFolder)
				folders.push_back(c);
			else
				files.push_back(c);
		}

		linatl::sort(folders.begin(), folders.end(), [](ResourceDirectory* folder, ResourceDirectory* other) -> bool { return folder->name < other->name; });
		linatl::sort(files.begin(), files.end(), [](ResourceDirectory* file, ResourceDirectory* other) -> bool { return file->name < other->name; });
		children.clear();
		children.insert(children.end(), folders.begin(), folders.end());
		children.insert(children.end(), files.begin(), files.end());
	}

	ResourceDirectory* ResourceDirectory::GetChildByName(const String& name)
	{
		auto it = linatl::find_if(children.begin(), children.end(), [name](ResourceDirectory* dir) -> bool { return name.compare(dir->name) == 0; });
		if (it == children.end())
			return nullptr;
		return *it;
	}

	ResourceDirectory* ResourceDirectory::FindResourceDirectory(ResourceID id)
	{
		if (!isFolder && resourceID == id)
			return this;

		for (ResourceDirectory* c : children)
		{
			ResourceDirectory* d = c->FindResourceDirectory(id);
			if (d != nullptr)
				return d;
		}

		return nullptr;
	}
} // namespace Lina
