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

#ifndef CommonResources_HPP
#define CommonResources_HPP

#include "Core/StringID.hpp"
#include "Data/String.hpp"
#include "Serialization/ISerializable.hpp"

namespace Lina
{
	enum class ResourceManagerMode
	{
		File,
		Package
	};

	enum class PackageType
	{
		Default,
		Package1,
		Package2,
	};

	extern String GGetPackagePath(PackageType pt);

	struct ResourceIdentifier : public ISerializable
	{
		ResourceIdentifier() = default;
		ResourceIdentifier(const String& path, TypeID tid, StringID sid)
		{
			this->path = path;
			this->sid  = sid;
			this->tid  = tid;
		}

		TypeID	 tid  = 0;
		StringID sid  = 0;
		String	 path = "";

		// Inherited via ISerializable
		virtual void SaveToStream(OStream& stream) override;
		virtual void LoadFromStream(IStream& stream) override;
	};

	struct ResourceLoadTask
	{
		Taskflow				   tf;
		Vector<ResourceIdentifier> identifiers;
		Atomic<bool>			   isCompleted = false;
		int32					   id		   = 0;
		uint64					   startTime   = 0;
		uint64					   endTime	   = 0;
	};

} // namespace Lina

#endif
