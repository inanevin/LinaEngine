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

#include "Core/ResourcePackager.hpp"
#include "Serialization/Archive.hpp"
#include "Serialization/Serialization.hpp"
#include "Core/ResourceManager.hpp"

namespace Lina::Resources
{
    Mutex ResourcePackager::m_mtx;

    void ResourcePackager::PackageFiles(PackageType type, const Vector<Pair<TypeID, String>>& resources)
    {
        if (!Utility::FileExists("Packages/"))
            Utility::CreateFolderInPath("Packages/");

        if (resources.empty())
            return;

        const String packageName = GetPackageTypeName(type);
        const String packagePath = "Packages/" + packageName + ".linapckg";
        auto*        rm          = Resources::ResourceManager::Get();

        Serialization::Archive<OStream> archive;

        for (auto& pair : resources)
        {
            const TypeID   tid  = pair.first;
            const String   path = pair.second;
            const StringID sid  = TO_SID(path);

            archive(tid);
            archive(sid);

            Resource* res = rm->GetCache(tid)->CreateMockResource();
            res->m_path   = path;
            res->m_sid    = sid;
            res->m_tid    = tid;
            res->WriteToPackage(archive);
            delete res;

            LOCK_GUARD(m_mtx);
            Event::EventSystem::Get()->Trigger<Event::EResourceProgressUpdated>(Event::EResourceProgressUpdated{.currentResource = path});
        }

        Serialization::SaveArchiveToFile(packagePath, archive);
    }
} // namespace Lina::Resources
