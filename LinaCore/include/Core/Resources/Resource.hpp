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

#include "Common/StringID.hpp"
#include "Common/Data/String.hpp"
#include "Core/Reflection/ResourceReflection.hpp"
#include "Common/ClassMacros.hpp"
#include "Common/Memory/CommonMemory.hpp"

namespace Lina
{
	namespace Editor
	{
		class Editor;
	}
	class System;

	class Resource
	{
	public:
		Resource(ResourceID id, TypeID tid, const String& name) : m_id(id), m_tid(tid), m_name(name){};
		virtual ~Resource();

		virtual void   GenerateHW(){};
		virtual void   DestroyHW(){};
		virtual void   SetCustomMeta(IStream& stream){};
		virtual size_t GetSize() const
		{
			return 0;
		};

		virtual bool LoadFromFile(const String& path)
		{
			return false;
		};
		virtual void LoadFromStream(IStream& stream)
		{
			stream >> m_id >> m_name >> m_path;
		};
		virtual void SaveToStream(OStream& stream) const
		{
			stream << m_id << m_name << m_path;
		};

		void SaveToFileAsBinary(const String& path);

		inline void SetID(ResourceID id)
		{
			m_id = id;
		}

		inline void SetName(const String& name)
		{
			m_name = name;
		}

		inline TypeID GetTID() const
		{
			return m_tid;
		}

		inline ResourceID GetID() const
		{
			return m_id;
		}

		inline const String& GetName() const
		{
			return m_name;
		}

		inline void SetPath(const String& path)
		{
			m_path = path;
		}

		inline const String& GetPath() const
		{
			return m_path;
		}

		inline bool IsHWValid() const
		{
			return m_hwValid;
		}

		inline bool IsHWUploadValid() const
		{
			return m_hwUploadValid;
		}

		virtual void SetSubdata(void* data){};

		inline uint32 GetBindlessIndex() const
		{
			return m_bindlessIndex;
		}

		inline void SetBindlessIndex(uint32 idx)
		{
			m_bindlessIndex = idx;
		}

	protected:
		template <typename U> friend class ResourceCache;

	protected:
		String	   m_name		   = "";
		String	   m_path		   = "";
		ResourceID m_id			   = 0;
		TypeID	   m_tid		   = 0;
		bool	   m_hwValid	   = false;
		bool	   m_hwUploadValid = false;
		uint32	   m_bindlessIndex = 0;
	};

	class ResRefBase
	{
	public:
		ResourceID id = 0;
	};

	template <typename T> class ResRef : public ResRefBase
	{
	public:
		T* raw = nullptr;

		void SaveToStream(OStream& stream) const
		{
			stream << id;
		};

		void LoadFromStream(IStream& stream)
		{
			stream >> id;
		};
	};

} // namespace Lina
