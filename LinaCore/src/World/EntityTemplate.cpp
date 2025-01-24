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

#include "Core/World/EntityTemplate.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Common/Data/Streams.hpp"
#include "Core/World/Entity.hpp"
#include "Core/World/WorldUtility.hpp"

namespace Lina
{

	bool EntityTemplate::LoadFromFile(const String& path)
	{
		IStream stream = Serialization::LoadFromFile(path.c_str());

		if (!stream.Empty())
			LoadFromStream(stream);
		else
			return false;

		stream.Destroy();
		return true;
	}

	void EntityTemplate::LoadFromStream(IStream& stream)
	{
		Resource::LoadFromStream(stream);
		uint32 version = 0;
		stream >> version;

		uint32 sz = 0;
		stream >> sz;

		if (sz > 0)
		{
			m_entityStream.Destroy();
			m_entityStream.Create(stream.GetDataCurrent(), static_cast<size_t>(sz));
			stream.SkipBy(static_cast<size_t>(sz));
		}
	}

	void EntityTemplate::SaveToStream(OStream& stream) const
	{
		Resource::SaveToStream(stream);
		stream << VERSION;

		const uint32 sz = static_cast<uint32>(m_entityStream.GetSize());
		stream << sz;

		if (sz > 0)
			stream.WriteRaw(m_entityStream.GetRaw(), m_entityStream.GetSize());
	}

	void EntityTemplate::GenerateHW()
	{
		LINA_ASSERT(m_hwValid == false, "");
		m_hwValid = true;
	}

	void EntityTemplate::DestroyHW()
	{
		LINA_ASSERT(m_hwValid, "");
		m_hwValid		= false;
		m_hwUploadValid = false;
	}

	void EntityTemplate::SetEntity(Entity* e)
	{
		m_entityStream.Destroy();
		OStream stream;
		WorldUtility::SaveEntitiesToStream(stream, e->GetWorld(), {e}, e->GetParent());
		m_entityStream.Create(stream.GetDataRaw(), stream.GetCurrentSize());
		stream.Destroy();
	}

	Vector<Entity*> EntityTemplate::CreateFromStream(EntityWorld* w)
	{
		IStream stream;
		stream.Create(m_entityStream.GetRaw(), m_entityStream.GetSize());
		Vector<Entity*> entities;
		WorldUtility::LoadEntitiesFromStream(stream, w, entities);
		stream.Destroy();
		return entities;
	}
} // namespace Lina
