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

#include "Core/Meta/ProjectData.hpp"
#include "Common/Data/Streams.hpp"
#include "Core/World/EntityWorld.hpp"

#include "Common/FileSystem/FileSystem.hpp"
#include <random>

namespace Lina
{

	ResourceID ProjectData::ConsumeResourceID()
	{
		static std::random_device rd;
		static std::mt19937_64	  generator(rd()); // 64-bit Mersenne Twister
		return generator() % RESOURCE_ID_CUSTOM_SPACE;
	}

	ResourceID ProjectData::ConsumeGlobalGUID()
	{
		static std::random_device rd;
		static std::mt19937_64	  generator(rd()); // 64-bit Mersenne Twister
		return generator() % RESOURCE_ID_CUSTOM_SPACE;
	}

	void ProjectData::LoadFromStream(IStream& in)
	{
		uint32 version = 0;
		in >> version;
		in >> m_resourceIDCounter;
		in >> m_projectName;
		in >> m_globalGUIDCounter;
		in >> m_settingsPackaging;
		m_rootDirectory.LoadFromStream(in, this);
	}

	void ProjectData::SaveToStream(OStream& out)
	{
		out << VERSION;
		out << m_resourceIDCounter;
		out << m_projectName;
		out << m_globalGUIDCounter;
		out << m_settingsPackaging;
		m_rootDirectory.SaveToStream(out);
	}

	void ProjectData::ToRelativePath(const String& absPath, String& outRelative)
	{
		const String basePath = FileSystem::GetFilePath(m_serializedPath);
		const size_t baseSz	  = basePath.size();
		outRelative			  = absPath.substr(baseSz, absPath.size());
	}

	String ProjectData::GetResourceDirectory()
	{
		auto p	= GetPath();
		auto aq = FileSystem::GetFilePath(p);
		return aq + "_LinaResourceCache/";
	}

	String ProjectData::GetResourcePath(ResourceID id)
	{
		return GetResourceDirectory() + "Resource_" + TO_STRING(id) + ".linaresource";
	}

	ResourceDirectory* ProjectData::CreateResourceDirectory(ResourceDirectory* parent, ResourceDirectory desc)
	{
		ResourceDirectory* dir		   = m_directoryBucket.Allocate();
		BucketIdentifier   bucketIdent = dir->m_bucketIdent;
		*dir						   = desc;
		dir->guid					   = ConsumeGlobalGUID();
		dir->m_bucketIdent			   = bucketIdent;

		parent->children.push_back(dir);
		dir->parent = parent;
		parent->SortChildren();

		LINA_TRACE("Created resource directory {0} {1}, parent {2} {3}", dir->name, (void*)dir, parent->name, (void*)parent);
		return dir;
	}

	void ProjectData::DestroyResourceDirectory(ResourceDirectory* dir)
	{
		ResourceDirectory* parent = dir->parent;
		LINA_TRACE("Destroying resource directory {0} {1}, parent {2} {3}", dir->name, (void*)dir, parent->name, (void*)parent);

		auto it = linatl::find_if(parent->children.begin(), parent->children.end(), [dir](ResourceDirectory* c) -> bool { return c == dir; });
		parent->children.erase(it);
		parent->SortChildren();

		FreeChildrenRecursive(dir);
		m_directoryBucket.Free(dir);
	}

	void ProjectData::FreeChildrenRecursive(ResourceDirectory* dir)
	{
		for (ResourceDirectory* d : dir->children)
		{
			FreeChildrenRecursive(d);
			m_directoryBucket.Free(d);
		}
	}

	ResourceDirectory* ProjectData::DuplicateResourceDirectory(ResourceDirectory* parent, ResourceDirectory* dir)
	{
		ResourceDirectory* dup	 = m_directoryBucket.Allocate();
		BucketIdentifier   ident = dup->m_bucketIdent;
		*dup					 = *dir;
		dup->guid				 = ConsumeGlobalGUID();
		dup->m_bucketIdent		 = ident;

		parent->children.push_back(dup);
		dup->parent = parent;
		return dup;
	}

	void ProjectData::DestroyChildDirectories(ResourceDirectory* dir)
	{
		for (ResourceDirectory* d : dir->children)
		{
			DestroyChildDirectories(d);
			m_directoryBucket.Free(d);
		}

		dir->children.clear();
	}
	void ProjectData::MoveResourceDirectory(ResourceDirectory* dir, ResourceDirectory* newParent)
	{
		ResourceDirectory* parent = dir->parent;
		auto			   it	  = linatl::find_if(parent->children.begin(), parent->children.end(), [dir](ResourceDirectory* c) -> bool { return c == dir; });
		parent->children.erase(it);
		dir->parent = newParent;
		newParent->children.push_back(dir);
		newParent->SortChildren();
	}

	LINA_CLASS_BEGIN(PackagingSettings)
	LINA_FIELD_VEC(PackagingSettings, worldIDsToPack, "Worlds", FieldType::ResourceID, ResourceID, GetTypeID<EntityWorld>());
	LINA_FIELD(PackagingSettings, executableName, "Executable Name", FieldType::String, 0)
	LINA_CLASS_END(PackagingSettings)
} // namespace Lina
