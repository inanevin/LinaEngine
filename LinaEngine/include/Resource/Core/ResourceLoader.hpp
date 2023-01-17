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

#ifndef DefaultResourceLoader_HPP
#define DefaultResourceLoader_HPP

// Headers here.
#include "Data/Vector.hpp"
#include "Data/String.hpp"
#include "Data/Mutex.hpp"
#include "Utility/StringId.hpp"
#include "ResourceCommon.hpp"
#include "Data/DataCommon.hpp"
#include "Core/CommonEngine.hpp"

namespace Lina
{
    class IStream;
}

namespace Lina::Resources
{
    struct TidSid
    {
        TypeID   tid = 0;
        StringID sid = 0;
    };

    class ResourceLoader
    {
    public:
        ResourceLoader(const EngineSubsystems& subsystems) : m_subsystems(subsystems){};
        virtual ~ResourceLoader() = default;

        virtual void LoadLevelResources(const Vector<Pair<TypeID, String>>& resources);
        virtual void LoadEngineResources();
        virtual void LoadSingleResource(TypeID tid, const char* path, bool async);

    protected:
        void LoadResources(PackageType packageType, const Vector<Pair<TypeID, String>>& resources, bool async);
        void LoadResourceFromMemory(TypeID tid, const char* path, uint8* data, uint32 size);

    protected:
        EngineSubsystems m_subsystems;

    private:
        Mutex m_mtx;
    };
} // namespace Lina::Resources

#endif
