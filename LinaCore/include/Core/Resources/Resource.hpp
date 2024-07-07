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
#include "Core/Resources/CommonResources.hpp"
#include "Core/Reflection/ClassReflection.hpp"
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
		Resource(const String& path, StringID sid, TypeID tid) : m_path(path), m_sid(sid), m_tid(tid){};
		virtual ~Resource() = default;

		virtual void LoadFromFile(const char* path){};
		virtual void LoadFromStream(IStream& stream){};
		virtual void SaveToStream(OStream& stream) const {};
		virtual void SetCustomMeta(IStream& stream){};
		void		 SaveToFileAsBinary(const String& path);

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

	protected:
		friend class ResourceManager;

		template <typename U> friend class ResourceCache;

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
		String	  m_path  = "";
		TypeID	  m_tid	  = 0;
		StringID  m_sid	  = 0;
		Bitmask32 m_flags = 0;
	};

	template <typename T> struct ResRef
	{
		T*		 raw = nullptr;
		StringID sid = 0;

		void SaveToStream(OStream& stream) const
		{
			stream << sid;
		};

		void LoadFromStream(IStream& stream)
		{
			stream >> sid;
		};
	};

} // namespace Lina
