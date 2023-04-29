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

#include "World/Level/Level.hpp"
#include "World/Core/EntityWorld.hpp"
#include "Serialization/VectorSerialization.hpp"

namespace Lina
{
	Level::~Level()
	{
		if (m_world)
		{
			Event ev = Event();
			m_world->DispatchEvent(EVG_End, ev);
			delete m_world;
		}

		m_worldStream.Destroy();
		m_loadedRuntimeUserStream.Destroy();
		m_runtimeUserStream.Destroy();
	}

	void Level::Install()
	{
		m_world = new EntityWorld();

		if (m_worldStream.GetSize() != 0)
		{
			m_world->LoadFromStream(m_worldStream);
			m_worldStream.Destroy();
		}

		m_runtimeUserStream.CreateReserve(4);
	}

	void Level::Uninstall()
	{
		Event ev = Event();
		m_world->DispatchEvent(EVG_End, ev);
		delete m_world;
		m_world = nullptr;
	}

	void Level::SaveToStream(OStream& stream)
	{
		if (m_world)
		{
			OStream worldStream;
			worldStream.CreateReserve(4);
			m_world->SaveToStream(worldStream);

			const uint32 worldStreamSize = static_cast<uint32>(worldStream.GetCurrentSize());
			stream << worldStreamSize;

			if (worldStreamSize > 0)
				stream.WriteEndianSafe(worldStream.GetDataRaw(), worldStream.GetCurrentSize());

			worldStream.Destroy();
		}
		else
		{
			const uint32 worldStreamSize = 0;
			stream << worldStreamSize;
		}

		VectorSerialization::SaveToStream_OBJ(stream, m_usedResources);

		const uint32 userStreamSize = static_cast<uint32>(m_runtimeUserStream.GetCurrentSize());
		stream << userStreamSize;

		if (userStreamSize > 0)
			stream.WriteEndianSafe(m_runtimeUserStream.GetDataRaw(), m_runtimeUserStream.GetCurrentSize());

		// Refresh after saving
		m_runtimeUserStream.Destroy();
		m_runtimeUserStream.CreateReserve(4);
	}

	void Level::LoadFromStream(IStream& stream)
	{
		uint32 worldSize = 0;
		stream >> worldSize;

		if (worldSize > 0)
		{
			m_worldStream.Create(worldSize);
			stream.ReadEndianSafe(m_worldStream.GetDataCurrent(), worldSize);
		}

		VectorSerialization::LoadFromStream_OBJ(stream, m_usedResources);

		uint32 userStreamSize = 0;
		stream >> userStreamSize;

		if (userStreamSize > 0)
			m_loadedRuntimeUserStream.Create(stream.GetDataCurrent(), userStreamSize);
	}

} // namespace Lina
