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
#include "Common/StringID.hpp"
#include "Common/Data/Streams.hpp"
#include "Common/Serialization/Serializable.hpp"
#include "Core/Resources/ResourceDirectory.hpp"

namespace Lina
{

	struct PackagingSettings
	{
		Vector<ResourceID> worldIDsToPack;
		String			   executableName = "Lina";

		virtual void SaveToStream(OStream& stream)
		{
			stream << worldIDsToPack << executableName;
		}
		virtual void LoadFromStream(IStream& stream)
		{
			stream >> worldIDsToPack >> executableName;
		}
	};

	class ProjectData : public Serializable
	{
	public:
		static constexpr uint32 VERSION = 0;

		virtual void	   SaveToStream(OStream& out) override;
		virtual void	   LoadFromStream(IStream& in) override;
		void			   ToRelativePath(const String& absPath, String& outRelative);
		String			   GetResourceDirectory();
		String			   GetResourcePath(ResourceID id);
		ResourceDirectory* CreateResourceDirectory(ResourceDirectory* parent, ResourceDirectory desc);
		void			   DestroyResourceDirectory(ResourceDirectory* dir);
		ResourceDirectory* DuplicateResourceDirectory(ResourceDirectory* parent, ResourceDirectory* dir);
		void			   DestroyChildDirectories(ResourceDirectory* dir);
		void			   MoveResourceDirectory(ResourceDirectory* dir, ResourceDirectory* newParent);

		inline void SetProjectName(const String& name)
		{
			m_projectName = name;
		}

		inline const String& GetProjectName() const
		{
			return m_projectName;
		}

		inline ResourceDirectory& GetResourceRoot()
		{
			return m_rootDirectory;
		}

		inline ResourceID ConsumeResourceID()
		{
			const ResourceID id = m_resourceIDCounter;
			m_resourceIDCounter++;
			return id;
		}

		inline GUID ConsumeGlobalGUID()
		{
			const GUID guid = m_globalGUIDCounter;
			m_globalGUIDCounter++;
			return guid;
		}

		inline PackagingSettings& GetSettingsPackaging()
		{
			return m_settingsPackaging;
		}

	private:
		void FreeChildrenRecursive(ResourceDirectory* dir);

	private:
		AllocatorBucket<ResourceDirectory, 1000> m_directoryBucket;
		String									 m_projectName		 = "";
		ResourceID								 m_resourceIDCounter = 1;
		GUID									 m_globalGUIDCounter = 1;
		Vector<ResourceID>						 m_worldsToPackage;
		ResourceDirectory						 m_rootDirectory = {
								   .name	 = "Resources",
								   .isFolder = true,
		   };
		PackagingSettings m_settingsPackaging = {};
	};

} // namespace Lina
