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

#include "Core/StringID.hpp"
#include "Data/String.hpp"
#include "Resources/Data/ResourceMetadata.hpp"

namespace Lina
{
	namespace Editor
	{
		class Editor;
	}
	class ResourceManager;
	class System;

	class Resource
	{
	public:
		Resource(ResourceManager* rm, bool isUserManaged, const String& path, StringID sid, TypeID tid);
		virtual ~Resource();

		inline const String& GetPath() const
		{
			return m_path;
		}

		inline StringID GetSID() const
		{
			return m_sid;
		}

		inline TypeID GetTID() const
		{
			return m_tid;
		}

		inline ResourceMetadata& GetMetadata()
		{
			return m_metadata;
		}

		inline bool IsUserManaged() const
		{
			return m_userManaged;
		}

	protected:
		friend class ResourceManager;
		friend class Editor::Editor;

		template <typename U> friend class ResourceCache;

		virtual void Flush(){};
		virtual void Upload(){};
		virtual void BatchLoaded(){};
		virtual void LoadFromFile(const char* path){};
		virtual void LoadFromStream(IStream& stream){};
		virtual void SaveToStream(OStream& stream){};

		inline void SetMetadata(const ResourceMetadata& md)
		{
			m_metadata = md;
		}

		inline void SetPath(const String& path)
		{
			m_path = path;
		}

		inline void SetSID(StringID sid)
		{
			m_sid = sid;
		}

		inline void SetTID(TypeID tid)
		{
			m_tid = tid;
		}

	protected:
		bool			 m_userManaged	   = false;
		ResourceManager* m_resourceManager = nullptr;
		ResourceMetadata m_metadata		   = ResourceMetadata();
		String			 m_path			   = "";
		TypeID			 m_tid			   = 0;
		StringID		 m_sid			   = 0;
	};
} // namespace Lina