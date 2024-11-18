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
#include "Core/Meta/ProjectData.hpp"

namespace Lina
{

	ResourceDirectory::~ResourceDirectory()
	{
	}

	void ResourceDirectory::SaveToStream(OStream& stream) const
	{
		stream << VERSION;
		stream << sourcePathRelativeToProject;
		stream << name;
		stream << isFolder;
		stream << resourceID;
		stream << resourceTID;
		stream << resourceType;
		stream << lastModifiedSID;
		stream << guid;
		stream << userData.directoryType;
		stream << userData.directoryMask;
		stream << userData.isInFavourites;
		stream << userData.unfolded;

		const uint32 sz = static_cast<uint32>(children.size());
		stream << sz;

		for (ResourceDirectory* c : children)
			c->SaveToStream(stream);
	}

	void ResourceDirectory::LoadFromStream(IStream& stream, ProjectData* projectData)
	{
		uint32 ver = 0;
		stream >> ver;
		stream >> sourcePathRelativeToProject;
		stream >> name;
		stream >> isFolder;
		stream >> resourceID;
		stream >> resourceTID;
		stream >> resourceType;
		stream >> lastModifiedSID;
		stream >> guid;
		stream >> userData.directoryType;
		stream >> userData.directoryMask;
		stream >> userData.isInFavourites;
		stream >> userData.unfolded;

		uint32 childSz = 0;
		stream >> childSz;

		for (uint32 i = 0; i < childSz; i++)
		{
			ResourceDirectory* dir = projectData->CreateResourceDirectory(this, {});
			dir->LoadFromStream(stream, projectData);
		}

		SortChildren();
	}

	void ResourceDirectory::SortChildren()
	{

		struct Sort
		{
			TypeID					   tid = 0;
			Vector<ResourceDirectory*> dirs;
		};

		Vector<Sort> sortVec;

		for (ResourceDirectory* c : children)
		{
			auto it = linatl::find_if(sortVec.begin(), sortVec.end(), [c](const Sort& sort) -> bool { return c->resourceTID == sort.tid; });
			if (it == sortVec.end())
				sortVec.push_back({.tid = c->resourceTID, .dirs = {c}});
			else
				it->dirs.push_back(c);
		}

		children.clear();

		linatl::sort(sortVec.begin(), sortVec.end(), [](Sort& sort0, Sort& sort1) -> bool { return sort0.tid < sort1.tid; });

		for (Sort& sort : sortVec)
		{
			linatl::sort(sort.dirs.begin(), sort.dirs.end(), [](ResourceDirectory* dir0, ResourceDirectory* dir1) -> bool { return dir0->name < dir1->name; });
			children.insert(children.end(), sort.dirs.begin(), sort.dirs.end());
		}
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

	ResourceDirectory* ResourceDirectory::FindByGUID(GUID g)
	{
		if (guid == g)
			return this;

		for (ResourceDirectory* c : children)
		{
			ResourceDirectory* d = c->FindByGUID(g);
			if (d != nullptr)
				return d;
		}

		return nullptr;
	}
} // namespace Lina
