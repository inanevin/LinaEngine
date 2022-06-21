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
        if (m_appMode == ApplicationMode::Editor) return;

        const String fullPath = "Packages/" + packageName + RESOURCEPACKAGE_EXTENSION;
        if (!Utility::FileExists(fullPath))
        {
            LINA_ERR("Package does not exist, aborting import. {0}", fullPath);
            return;
        }

        String fullBundlePath = fullPath;
        ResourceStorage::s_currentProgressData.Reset();
        ResourceStorage::s_currentProgressData.m_progressTitle       = "Unpacking package: " + packageName;
        ResourceStorage::s_currentProgressData.m_currentResourceName = fullBundlePath;
        Event::EventSystem::Get()->Trigger<Event::EResourceProgressStarted>();

        // Start unpacking.
        UnpackAndLoad(fullBundlePath, pass, loader);
        loader->LoadAllMemoryResources();

        // Set progress end.
        ResourceStorage::s_currentProgressData.Reset();
        Event::EventSystem::Get()->Trigger<Event::EResourceProgressEnded>();
    }

    void ResourcePackager::LoadFilesFromPackage(const String& packageName, const HashSet<StringIDType>& filesToLoad, const wchar_t* pass, ResourceLoader* loader)
    {
        if(m_appMode == ApplicationMode::Editor) return;
        
        const String fullPath = "Packages/" + packageName + RESOURCEPACKAGE_EXTENSION;
        if (!Utility::FileExists(fullPath))
        {
            LINA_ERR("Package does not exist, aborting import. {0}", fullPath);
            return;
        }

        String fullBundlePath = fullPath;
        ResourceStorage::s_currentProgressData.Reset();
        ResourceStorage::s_currentProgressData.m_progressTitle       = "Unpacking package: " + packageName;
        ResourceStorage::s_currentProgressData.m_currentResourceName = fullBundlePath;
        Event::EventSystem::Get()->Trigger<Event::EResourceProgressStarted>();

        // Start unpacking.
        UnpackAndLoad(fullBundlePath, filesToLoad, pass, loader);
        loader->LoadAllMemoryResources();

        // Set progress end.
        ResourceStorage::s_currentProgressData.Reset();
        Event::EventSystem::Get()->Trigger<Event::EResourceProgressEnded>();
    }

    void ResourcePackager::PackageProject(const String& path, const Vector<String>& levelResources, const HashMap<TypeID, HashSet<StringIDType>>& resourceMap)
    {
        if (m_appMode == ApplicationMode::Standalone) return;

        ResourceStorage::Get()->ScanRootFolder();
        auto*          storage = ResourceStorage::Get();
        Vector<String> filesToPack;
        auto&          allTypes = storage->GetResourceTypes();

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
                    String fullpath = ResourcesUtility::SearchFolderForSID(m_rootFolder, pair.first);
                    if (fullpath.compare("") != 0)
                        filesToPack.push_back(fullpath);
                }
            }
        }

        const String workingPath = path.compare("") == 0 ? "Packages/" : (path + "/Packages/");
        if (Utility::FileExists(workingPath))
            Utility::DeleteDirectory(workingPath);
        Utility::CreateFolderInPath(workingPath);

        m_packager.PackageFileset(filesToPack, workingPath + +"static" + RESOURCEPACKAGE_EXTENSION, m_appInfo.m_packagePass);

        // Pack the level resources, level resources might not be alive in the resource storage.
        // Thus use the table sent by the engine to figure out what to pack.
        m_packager.PackageFileset(levelResources, workingPath + "levels" + RESOURCEPACKAGE_EXTENSION, m_appInfo.m_packagePass);

        // Now for all resource types that are marked as non-static, package them into their respective packages.
        // We do not use the storage to get the resources here, we merely use it to check the type data, which is guaranteed to be registered.
        // We package the files based on the resource table sent by the engine here.
        for (const auto& [tid, sidVector] : resourceMap)
        {
            ResourceTypeData& typeData = storage->GetTypeData(tid);

            if (typeData.packageType != PackageType::Static)
            {
                filesToPack.clear();

                // Find the full paths & add them based on sids.
                for (auto sid : sidVector)
                {
                    String fullpath = SearchFolderForSID(m_rootFolder, sid);
                    if (fullpath.compare("") != 0)
                        filesToPack.push_back(fullpath);
                }

                m_packager.PackageFileset(filesToPack, workingPath + storage->PackageTypeToString(typeData.packageType) + RESOURCEPACKAGE_EXTENSION, m_appInfo.m_packagePass);
            }
        }
    }
    void ResourcePackager::PackageFileset(Vector<String> files, const String& output, const wchar_t* pass)
    {
        if (m_appMode == ApplicationMode::Standalone) return;

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
            compressor.setCompressionMethod(bit7z::BitCompressionMethod::Lzma);
            compressor.setPassword(pass);

            // Progress callback.
            bit7z::BitCompressor* pCompressor = &compressor;
            ResourceProgressData* loadingData = &ResourceStorage::s_currentProgressData;
            loadingData->Reset();
            loadingData->m_currentResourceName = "Packing directory";
            loadingData->m_progressTitle       = "Packing multiple files...";

            compressor.setTotalCallback([=](uint64_t size) {
                pCompressor->setProgressCallback([size, loadingData](uint64_t processedSize) {
                    loadingData->m_currentProgress = ((100.0f * (float)processedSize) / (float)size);
                    Event::EventSystem::Get()->Trigger<Event::EResourceProgressUpdated>(Event::EResourceProgressUpdated{.m_currentResource = loadingData->m_currentResourceName, .m_percentage = loadingData->m_currentProgress});
                });
            });

            compressor.setFileCallback([=](std::wstring file) {
                char* chr                          = Utility::WCharToChar(file.c_str());
                loadingData->m_currentResourceName = String(chr);
                Event::EventSystem::Get()->Trigger<Event::EResourceProgressUpdated>(Event::EResourceProgressUpdated{.m_currentResource = loadingData->m_currentResourceName, .m_percentage = loadingData->m_currentProgress});
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

            ResourceProgressData* loadingData = &ResourceStorage::s_currentProgressData;
            loadingData->Reset();
            auto  wFirstItemName               = arc.getItemProperty(0, bit7z::BitProperty::Path).getString();
            char* firstItemName                = Utility::WCharToChar(wFirstItemName.c_str());
            loadingData->m_currentResourceName = String(firstItemName);
            loadingData->m_progressTitle       = "Unpacking file " + filePath;
            Event::EventSystem::Get()->Trigger<Event::EResourceProgressUpdated>(Event::EResourceProgressUpdated{loadingData->m_currentResourceName, loadingData->m_currentProgress});
            delete[] firstItemName;

            int fileCounter = 0;

            extractor.setFileCallback([&](std::wstring file) {
                char* chr                          = Utility::WCharToChar(file.c_str());
                loadingData->m_currentResourceName = String(chr);
                loadingData->m_currentProgress     = ((100.0f * (float)fileCounter) / (float)totalFiles);
                fileCounter++;
                Event::EventSystem::Get()->Trigger<Event::EResourceProgressUpdated>(Event::EResourceProgressUpdated{.m_currentResource = loadingData->m_currentResourceName, .m_percentage = loadingData->m_currentProgress});
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
        int loadedFiles = 0;
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

            // Progress callback.
            ResourceProgressData* loadingData = &ResourceStorage::s_currentProgressData;
            loadingData->Reset();
            loadingData->m_currentTotalFiles = totalFiles;
            loadingData->m_progressTitle     = "Unpacking file " + filePath;

            // Clear the current memory queue in the loader.
            MemoryQueue empty;
            loader->m_memoryResources.swap(empty);

            Vector<bit7z::byte_t>      v;
            std::vector<bit7z::byte_t> stdv;

            // For every single file to be extracted, search all items & find the matching item index.
            // Then extract that item into memory from the archive.
            for (auto& file : filesToLoad)
            {
                for (auto& item : items)
                {
                    const char* path    = Utility::WCharToChar(item.path().c_str());
                    String      pathStr = path;
                    std::replace(pathStr.begin(), pathStr.end(), '\\', '/');

                    if (StringID(pathStr.c_str()).value() == file)
                    {
                        loadingData->m_currentResourceName = pathStr;
                        loadingData->m_currentProgress     = (loadedFiles / static_cast<float>(loadingData->m_currentTotalFiles)) * 100.0f;
                        Event::EventSystem::Get()->Trigger<Event::EResourceProgressUpdated>(Event::EResourceProgressUpdated{.m_currentResource = loadingData->m_currentResourceName, .m_percentage = loadingData->m_currentProgress});

                        extractor.extract(wdir, stdv, item.index());

                        for (int i = 0; i < stdv.size(); i++)
                            v.push_back(stdv[i]);

                        loader->PushResourceFromMemory(pathStr, v);

                        loadedFiles++;
                        stdv.clear();
                        v.clear();
                        break;
                    }

                    delete[] path;
                }
            }
        }
        catch (const bit7z::BitException& ex)
        {
            LINA_ERR("[Packager] -> Failed unpacking file {0} {1}", filePath, ex.what());
        }

        assert(loadedFiles == filesToLoad.size());
        LINA_INFO("[Packager] -> Successfully unpacked package: {0}", filePath);
    }

} // namespace Lina::Resources
