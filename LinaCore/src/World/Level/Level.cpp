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
            delete m_world;

        m_worldStream.Destroy();
    }

    void Level::Install(IEventDispatcher* dispatcher)
    {
        m_world = new EntityWorld(dispatcher);
        m_world->LoadFromStream(m_worldStream);
        m_worldStream.Destroy();

        Event data;
        data.pParams[0] = static_cast<void*>(m_world);
        dispatcher->DispatchGameEvent(EVG_LevelInstalled, data);

        data = Event();
        dispatcher->DispatchGameEvent(EVG_Start, data);
        dispatcher->DispatchGameEvent(EVG_PostStart, data);
    }

    void Level::Uninstall(IEventDispatcher* dispatcher)
    {
        Event data;
        data.pParams[0] = static_cast<void*>(m_world);
        dispatcher->DispatchGameEvent(EVG_LevelUninstalled, data);
        delete m_world;
    }

    void Level::SaveToStream(OStream& stream)
    {
        // custom data.
        VectorSerialization::SaveToStream_OBJ(stream, m_usedResources);

        // world.
        const size_t streamSize = stream.GetCurrentSize();
        m_world->SaveToStream(stream);
        const size_t totalSize = stream.GetCurrentSize();
        const uint32 worldSize = static_cast<uint32>(totalSize - streamSize);
        stream << worldSize;
    }

    void Level::LoadFromStream(IStream& stream)
    {
        uint32 worldSize = 0;
        stream.Seek(stream.GetSize() - sizeof(uint32));
        stream.Read(worldSize);
        stream.Seek(0);

        // custom data
        VectorSerialization::LoadFromStream_OBJ(stream, m_usedResources);

        // world.
        m_worldStream.Create(stream.GetDataCurrent(), worldSize);
    }

} // namespace Lina
