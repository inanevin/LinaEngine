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

#ifndef ResourceHandle_HPP
#define ResourceHandle_HPP

// Headers here.
#include "Resource/Core/ResourceManager.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Data/Set.hpp"

namespace Lina::Resources
{

    class ResourceHandleBase
    {
    public:
        ResourceHandleBase()          = default;
        virtual ~ResourceHandleBase() = default;

        virtual void OnResourceLoaded()   = 0;
        virtual void OnResourceUnloaded() = 0;
        virtual bool IsValid() const      = 0;

        void OnResourceSIDChanged(StringID resSidNew)
        {
            sid = resSidNew;
        }

        StringID sid = 0;
        TypeID   tid = 0;
    };

    template <typename T>
    class ResourceHandle : public ResourceHandleBase
    {
    public:
        T* value = nullptr;

        ResourceHandle()
        {
            tid = GetTypeID<T>();
        }
        virtual ~ResourceHandle()
        {
            Resources::ResourceManager::Get()->GetCache(tid)->RemoveResourceHandle(this);
        }

        template <class Archive>
        void Save(Archive& archive)
        {
            archive(sid);
        }

        template <class Archive>
        void Load(Archive& archive)
        {
            archive(sid);
            tid = GetTypeID<T>();

            if (sid != 0)
            {
                auto* manager = Resources::ResourceManager::Get();
                manager->GetCache(tid)->AddResourceHandle(this);

                if (manager->Exists<T>(sid))
                    value = manager->GetResource<T>(sid);
                else
                    value = nullptr;
            }
        }

        virtual void OnResourceLoaded() override
        {
            if (value != nullptr)
                return;
            value = Resources::ResourceManager::Get()->GetResource<T>(sid);
        }

        virtual void OnResourceUnloaded() override
        {
            if (value == nullptr)
                return;
            value = nullptr;
        }

        virtual bool IsValid() const override
        {
            return sid != 0 && value != nullptr;
        }

    private:
    };

} // namespace Lina::Resources

#endif
