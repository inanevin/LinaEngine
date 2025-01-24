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

#include "Common/Data/Streams.hpp"
#include "Core/Resources/CommonResources.hpp"
#include "Common/Memory/AllocatorBucket.hpp"

namespace Lina
{
	class OStream;
	class IStream;
	struct TextureAtlasImage;
	class ProjectData;

	// Don't like editor leaking here, but makes our lives so much easier.
	struct ResourceUserData
	{
		uint32	  directoryType	 = 0;
		Bitmask32 directoryMask	 = 0;
		bool	  isInFavourites = false;
		bool	  unfolded		 = false;
	};

	struct ResourceDirectory
	{
		static constexpr uint32 VERSION = 0;
		~ResourceDirectory();

		String					   name						   = "";
		String					   sourcePathRelativeToProject = "";
		bool					   isFolder					   = false;
		ResourceID				   resourceID				   = 0;
		TypeID					   resourceTID				   = 0;
		Vector<ResourceDirectory*> children;
		ResourceDirectory*		   parent		   = nullptr;
		ResourceType			   resourceType	   = ResourceType::ExternalSource;
		StringID				   lastModifiedSID = 0;
		GUID					   guid			   = 0;
		ResourceUserData		   userData		   = {};

		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream, ProjectData* projectData);

		ResourceDirectory* GetChildByName(const String& name);
		ResourceDirectory* FindResourceDirectory(ResourceID id);
		ResourceDirectory* FindByGUID(GUID guid);
		void			   SortChildren();

		ALLOCATOR_BUCKET_MEM;
	};
} // namespace Lina
