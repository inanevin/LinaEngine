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
#include "Core/ResourceStorage.hpp"
#include "Loaders/ResourceLoader.hpp"
#include "Data/Serialization/VectorSerialization.hpp"
#include "Log/Log.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Utility/ResourceUtility.hpp"

#include <bit7z/include/bitcompressor.hpp>
#include <bit7z/include/bitexception.hpp>
#include <bit7z/include/bitextractor.hpp>
#include <bit7z/include/bitformat.hpp>
#include <bit7z/include/bitmemextractor.hpp>
#include <bit7z/include/bittypes.hpp>
#include <bit7z/include/bitarchiveinfo.hpp>

namespace Lina::Resources
{
#define RESOURCEPACKAGE_EXTENSION String(".linapackage")

    void ResourcePackager::LoadPackage(const String& packageName, const wchar_t* pass, ResourceLoader* loader)
    {

        const String fullPath = "Packages/" + packageName + RESOURCEPACKAGE_EXTENSION;
        if (!Utility::FileExists(fullPath))
        {
            LINA_ERR("Package does not exist, aborting import. {0}", fullPath);
            return;
        }

        String fullBundlePath = fullPath;

        // Start unpacking.
        UnpackAndLoad(fullBundlePath, pass, loader);
        loader->LoadAllMemoryResources();
    }

    void ResourcePackager::LoadFilesFromPackage(const String& packageName, const HashSet<StringIDType>& filesToLoad, const wchar_t* pass, ResourceLoader* loader)
    {
        const String fullPath = "Packages/" + packageName + RESOURCEPACKAGE_EXTENSION;
        if (!Utility::FileExists(fullPath))
        {
            LINA_ERR("Package does not exist, aborting import. {0}", fullPath);
            return;
        }

        String fullBundlePath = fullPath;

        // Start unpacking.
        UnpackAndLoad(fullBundlePath, filesToLoad, pass, loader);
        loader->LoadAllMemoryResources();
    }

    void ResourcePackager::UnpackAndLoad(const String& filePath, const wchar_t* pass, ResourceLoader* loader)
    {
        try
        {
            // Delete first if exists.
            if (!Utility::FileExists(filePath))
            {
                LINA_ERR("[Packager] -> Failed unpacking file, file does not exist: {0}", filePath);
                return;
            }

            LINA_INFO("[Packager] -> Starting unpacking package: {0}", filePath);

            // Setup extractor.
            bit7z::Bit7zLibrary lib{L"7z.dll"};
            bit7z::BitExtractor extractor{lib, bit7z::BitFormat::SevenZip};
            extractor.setPassword(pass);

            // Setup path.
            const char*  filePathChr = filePath.c_str();
            const size_t dirSize     = strlen(filePathChr) + 1;
            std::wstring wdir(dirSize, L'#');
            size_t       numConverted;
            mbstowcs_s(&numConverted, &wdir[0], dirSize, filePathChr, dirSize - 1);

            bit7z::BitArchiveInfo arc{lib, wdir, bit7z::BitFormat::SevenZip};
            auto                  items      = arc.items();
            const int             totalFiles = arc.itemsCount();

            if (totalFiles == 0)
            {
                LINA_ERR("[Packager] -> Package is empty: {0}", filePath);
                return;
            }

            extractor.setFileCallback([&](std::wstring file) {
                char* chr = Utility::WCharToChar(file.c_str());
                Event::EventSystem::Get()->Trigger<Event::EResourceProgressUpdated>(Event::EResourceProgressUpdated{.currentResource = String(chr)});
                delete[] chr;
            });

            // Extract.
            std::map<std::wstring, std::vector<bit7z::byte_t>> map;
            extractor.extract(wdir, map);

            // Clear the current memory queue in the loader.
            MemoryQueue empty;
            loader->m_memoryResources.swap(empty);

            // Sort the resources into their respective packages in the loader.
            for (auto& item : map)
            {
                // Setup paths
                const char* filePath    = Utility::WCharToChar(item.first.c_str());
                String      filePathStr = filePath;
                std::replace(filePathStr.begin(), filePathStr.end(), '\\', '/');

                // Pass the resource to loader.
                Vector<bit7z::byte_t> v;
                v.reserve(item.second.size());
                for (int i = 0; i < item.second.size(); i++)
                    v.push_back(item.second[i]);

                loader->PushResourceFromMemory(filePathStr, v);
                delete[] filePath;
            }
        }
        catch (const bit7z::BitException& ex)
        {
            LINA_ERR("[Packager] -> Failed unpacking file {0} {1}", filePath, ex.what());
        }

        LINA_INFO("[Packager] -> Successfully unpacked package: {0}", filePath);
    }

    void ResourcePackager::UnpackAndLoad(const String& filePath, const HashSet<StringIDType>& filesToLoad, const wchar_t* pass, ResourceLoader* loader)
    {
#ifdef LINA_MT
        Atomic<int> loadedFiles = 0;
#else
        int loadedFiles = 0;
#endif
        try
        {
            // Delete first if exists.
            if (!Utility::FileExists(filePath))
            {
                LINA_ERR("[Packager] -> Failed unpacking file, file does not exist: {0}", filePath);
                return;
            }

            LINA_INFO("[Packager] -> Started unpacking package: {0}", filePath);

            // Setup extractor.
            bit7z::Bit7zLibrary lib{L"7z.dll"};
            bit7z::BitExtractor extractor{lib, bit7z::BitFormat::SevenZip};
            extractor.setPassword(pass);

            // Setup path.
            const char*  filePathChr = filePath.c_str();
            const size_t dirSize     = strlen(filePathChr) + 1;
            std::wstring wdir(dirSize, L'#');
            size_t       numConverted;
            mbstowcs_s(&numConverted, &wdir[0], dirSize, filePathChr, dirSize - 1);

            bit7z::BitArchiveInfo arc{lib, wdir, bit7z::BitFormat::SevenZip};
            auto                  items      = arc.items();
            const int             totalFiles = arc.itemsCount();

            if (totalFiles == 0)
            {
                LINA_ERR("[Packager] -> Package is empty: {0}", filePath);
                return;
            }

            // Clear the current memory queue in the loader.
            MemoryQueue empty;
            loader->m_memoryResources.swap(empty);

            // Determine which item indices to load.
            Vector<Pair<uint32_t, String>> itemIndices;
            for (auto& item : items)
            {
                const char* path    = Utility::WCharToChar(item.path().c_str());
                String      pathStr = path;
                std::replace(pathStr.begin(), pathStr.end(), '\\', '/');

                auto it = linatl::find_if(filesToLoad.begin(), filesToLoad.end(), [&](const auto& file) {
                    return file == StringID(pathStr.c_str()).value();
                });

                if (it != filesToLoad.end())
                    itemIndices.push_back(linatl::make_pair(item.index(), pathStr));

                delete[] path;
            }

            auto load = [&](uint32_t index, const String& str) {
                std::vector<bit7z::byte_t> stdv;
                extractor.extract(wdir, stdv, index);
                Event::EventSystem::Get()->Trigger<Event::EResourceProgressUpdated>(Event::EResourceProgressUpdated{.currentResource = str});

                Vector<bit7z::byte_t> v;
                for (int i = 0; i < stdv.size(); i++)
                    v.push_back(stdv[i]);

                loader->PushResourceFromMemory(str, v);

                loadedFiles++;
                stdv.clear();
                v.clear();
            };

#ifdef LINA_MT
            Taskflow taskflow;
            taskflow.for_each(itemIndices.begin(), itemIndices.end(), [&](const Pair<uint32_t, String>& pair) { load(pair.first, pair.second); });
            JobSystem::Get()->Run(taskflow).wait();
#else
            for (auto& pair : itemIndices)
                load(pair.first, pair.second);
#endif
        }
        catch (const bit7z::BitException& ex)
        {
            LINA_ERR("[Packager] -> Failed unpacking file {0} {1}", filePath, ex.what());
        }

        assert(loadedFiles == filesToLoad.size());
        LINA_INFO("[Packager] -> Successfully loaded files from package: {0}", filePath);
    }

    void ResourcePackager::PackageProject(const String& path, const Vector<String>& levelResources, const HashMap<TypeID, Vector<String>>& resourceMap, const wchar_t* pass)
    {
        ResourceUtility::ScanRootFolder();
        auto*          storage = ResourceStorage::Get();
        Vector<String> filesToPack;
        auto&          allTypes = storage->GetResourceTypes();

        Event::EventSystem::Get()->Trigger<Event::EResourceProgressStarted>(Event::EResourceProgressStarted{.title = "Packaging project...", .totalFiles = 0});

        // 1: Pack static resources
        // 2: Pack level data
        // 3: Pack all resources referenced by the levels.

        // Pack the static resources, all static resources should be loaded into the memory during the lifetime of the editor.
        // So we can find them from the resource cache.
        for (const auto& [tid, typeData] : allTypes)
        {
            if (typeData.packageType == PackageType::Static)
            {
                const Cache& cache = storage->GetCache(tid);

                for (auto& pair : cache)
                {
                    String fullpath = ResourceUtility::SearchFolderForSID(ResourceUtility::s_rootFolder, pair.first);
                    if (fullpath.compare("") != 0)
                        filesToPack.push_back(fullpath);
                }
            }
        }

        const String workingPath = path.compare("") == 0 ? "Packages/" : (path + "/Packages/");
        if (Utility::FileExists(workingPath))
            Utility::DeleteDirectory(workingPath);
        Utility::CreateFolderInPath(workingPath);

        PackageFileset(filesToPack, workingPath + "static" + RESOURCEPACKAGE_EXTENSION, pass);

        // Pack the level resources, level resources might not be alive in the resource storage.
        // Thus use the table sent by the engine to figure out what to pack.
        PackageFileset(levelResources, workingPath + "levels" + RESOURCEPACKAGE_EXTENSION, pass);

        // Now for all resource types that are marked as non-static, package them into their respective packages.
        // We do not use the storage to get the resources here, we merely use it to check the type data, which is guaranteed to be registered.
        // We package the files based on the resource table sent by the engine here.
        for (const auto& [tid, vector] : resourceMap)
        {
            ResourceTypeData& typeData = storage->GetTypeData(tid);

            if (typeData.packageType != PackageType::Static && typeData.packageType != PackageType::Level)
            {
                filesToPack.clear();

                // Find the full paths & add them based on sids.
                for (auto fullpath : vector)
                    filesToPack.push_back(fullpath);

                PackageFileset(filesToPack, workingPath + ResourceUtility::PackageTypeToString(typeData.packageType) + RESOURCEPACKAGE_EXTENSION, pass);
            }
        }
    }

    void ResourcePackager::PackageFileset(Vector<String> files, const String& output, const wchar_t* pass)
    {
        try
        {
            if (files.empty())
            {
                LINA_TRACE("[Packager] -> No files found in fileset, aborting packing...");
                return;
            }

            LINA_INFO("[Packager] -> Started packaging process: {0}", output);

            // Delete first if exists.
            if (Utility::FileExists(output))
                Utility::DeleteFileInPath(output);

            // Setup compression.
            bit7z::Bit7zLibrary  lib{L"7z.dll"};
            bit7z::BitCompressor compressor{lib, bit7z::BitFormat::SevenZip};
            compressor.setCompressionMethod(bit7z::BitCompressionMethod::Lzma2);
            compressor.setCompressionLevel(bit7z::BitCompressionLevel::ULTRA);
            compressor.setPassword(pass);

            // Progress callback.
            bit7z::BitCompressor* pCompressor = &compressor;

            compressor.setFileCallback([=](std::wstring file) {
                char* chr = Utility::WCharToChar(file.c_str());
                Event::EventSystem::Get()->Trigger<Event::EResourceProgressUpdated>(Event::EResourceProgressUpdated{.currentResource = String(chr)});
                delete chr;
            });

            // Create a vector of unicode file paths.
            Vector<std::wstring> wfiles;
            for (auto& file : files)
            {
                const size_t size = strlen(file.c_str()) + 1;
                std::wstring wfile(size, L'#');
                size_t       numConverted;
                mbstowcs_s(&numConverted, &wfile[0], size, file.c_str(), size - 1);
                wfiles.push_back(wfile);
            }

            // Setup output path.
            const char*  outputchr = output.c_str();
            const size_t outSize   = strlen(outputchr) + 1;
            std::wstring woutput(outSize, L'#');
            size_t       numConverted;
            mbstowcs_s(&numConverted, &woutput[0], outSize, outputchr, outSize - 1);

            // compress.
            std::vector<std::wstring> vfiles;
            vfiles.reserve(wfiles.size());
            for (int i = 0; i < wfiles.size(); i++)
                vfiles.push_back(wfiles[i]);

            compressor.compress(vfiles, woutput);
        }
        catch (const bit7z::BitException& ex)
        {
            LINA_ERR("[Packager] -> Failed packaging files {0} {1}", files[0], ex.what());
        }

        LINA_INFO("[Packager] -> Successfully completed packaging: {0}", output);
    }

} // namespace Lina::Resources
