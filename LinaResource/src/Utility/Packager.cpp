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

#include "Utility/Packager.hpp"
#include "Data/Serialization/VectorSerialization.hpp"
#include "Core/ResourceManager.hpp"
#include "Log/Log.hpp"
#include "Utility/UtilityFunctions.hpp"

#include <bit7z/include/bitcompressor.hpp>
#include <bit7z/include/bitexception.hpp>
#include <bit7z/include/bitextractor.hpp>
#include <bit7z/include/bitformat.hpp>
#include <bit7z/include/bitmemextractor.hpp>
#include <bit7z/include/bittypes.hpp>
#include <bit7z/include/bitarchiveinfo.hpp>

namespace Lina::Resources
{
    void Packager::PackageDirectory(const String& dir, const String& output, const wchar_t* pass)
    {
        try
        {
            // Delete first if exists.
            if (Utility::FileExists(output))
                Utility::DeleteFileInPath(output);

            // Setup compression.
            bit7z::Bit7zLibrary  lib{L"7z.dll"};
            bit7z::BitCompressor compressor{lib, bit7z::BitFormat::SevenZip};
            compressor.setCompressionMethod(bit7z::BitCompressionMethod::Lzma);
            compressor.setPassword(pass);

            // Progress callback.
            bit7z::BitCompressor* pCompressor  = &compressor;
            ResourceProgressData* loadingData  = &ResourceManager::s_currentProgressData;
            loadingData->m_currentResourceName = "Packing directory";
            loadingData->m_currentProgress     = 0;
            loadingData->m_state               = ResourceProgressState::Pending;
            loadingData->m_progressTitle       = "Packing directory" + String(dir);

            compressor.setTotalCallback([=](uint64_t size) {
                loadingData->m_state = ResourceProgressState::Pending;
                pCompressor->setProgressCallback([size, loadingData](uint64_t processedSize) { loadingData->m_currentProgress = ((100.0f * (float)processedSize) / (float)size); });
            });

            compressor.setFileCallback([=](std::wstring file) {
                char* chr                          = Utility::WCharToChar(file.c_str());
                loadingData->m_currentResourceName = String(chr);
                delete chr;
            });

            // Setup path.
            const char*  dirchr     = dir.c_str();
            const char*  outputchr  = output.c_str();
            const size_t dirSize    = strlen(dirchr);
            const size_t outputSize = strlen(outputchr);
            std::wstring wdir(dirSize, L'#');
            std::wstring woutput(outputSize, L'#');
            size_t       numConverted;
            size_t       numConverted2;
            mbstowcs_s(&numConverted, &wdir[0], dirSize, dirchr, dirSize - 1);
            mbstowcs_s(&numConverted2, &wdir[0], outputSize, outputchr, outputSize - 1);

            // compress.
            compressor.compressDirectory(wdir, woutput);

            LINA_TRACE("[Packager] -> Successfully packed directory {0}", dir);
        }
        catch (const bit7z::BitException& ex)
        {
            LINA_ERR("[Packager] -> Failed packaging directory {0} {1}", dir, ex.what());
        }
    }

    void Packager::PackageFileset(Vector<String> files, const String& output, const wchar_t* pass)
    {
        try
        {
            if (files.empty())
            {
                LINA_TRACE("[Packager] -> No files found in fileset, aborting packing...");
                return;
            }
            // Delete first if exists.
            if (Utility::FileExists(output))
                Utility::DeleteFileInPath(output);

            // Setup compression.
            bit7z::Bit7zLibrary  lib{L"7z.dll"};
            bit7z::BitCompressor compressor{lib, bit7z::BitFormat::SevenZip};
            compressor.setCompressionMethod(bit7z::BitCompressionMethod::Lzma);
            compressor.setPassword(pass);

            // Progress callback.
            bit7z::BitCompressor* pCompressor  = &compressor;
            ResourceProgressData* loadingData  = &ResourceManager::s_currentProgressData;
            loadingData->m_currentResourceName = "Packing directory";
            loadingData->m_currentProgress     = 0.0f;
            loadingData->m_state               = ResourceProgressState::Pending;
            loadingData->m_progressTitle       = "Packing multiple files...";

            compressor.setTotalCallback([=](uint64_t size) {
                loadingData->m_state = ResourceProgressState::Pending;
                pCompressor->setProgressCallback([size, loadingData](uint64_t processedSize) { loadingData->m_currentProgress = ((100.0f * (float)processedSize) / (float)size); });
            });

            compressor.setFileCallback([=](std::wstring file) {
                char* chr                          = Utility::WCharToChar(file.c_str());
                loadingData->m_currentResourceName = String(chr);
                LINA_INFO("[Packager] -> Packing {0}", loadingData->m_currentResourceName);
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
            LINA_INFO("[Packager] -> Successfully packed files to: {0}", output);
        }
        catch (const bit7z::BitException& ex)
        {
            LINA_ERR("[Packager] -> Failed packaging files {0} {1}", files[0], ex.what());
        }
    }

    void Packager::UnpackAndLoad(const String& filePath, const wchar_t* pass, ResourceLoader* loader)
    {
        try
        {
            // Delete first if exists.
            if (!Utility::FileExists(filePath))
            {
                LINA_ERR("[Packager] -> Failed unpacking file, file does not exist: {0}", filePath);
                return;
            }

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

            // Progress callback.
            bit7z::BitExtractor*  pExtractor   = &extractor;
            ResourceProgressData* loadingData  = &ResourceManager::s_currentProgressData;
            loadingData->m_currentResourceName = "nofile";
            loadingData->m_currentProgress     = 0;
            loadingData->m_state               = ResourceProgressState::Pending;
            loadingData->m_progressTitle       = "Unpacking file " + filePath;

            extractor.setTotalCallback([=](uint64_t size) {
                loadingData->m_state = ResourceProgressState::Pending;
                pExtractor->setProgressCallback([size, loadingData](uint64_t processedSize) { loadingData->m_currentProgress = ((100.0f * (float)processedSize) / (float)size); });
            });

            extractor.setFileCallback([=](std::wstring file) {
                char* chr                          = Utility::WCharToChar(file.c_str());
                loadingData->m_currentResourceName = String(chr);
                LINA_INFO("[Packager] -> Unpacking: {0}", loadingData->m_currentResourceName);
                delete chr;
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
            }

            LINA_INFO("[Packager] -> Successfully unpacked package: {0}", filePath);
        }
        catch (const bit7z::BitException& ex)
        {
            LINA_ERR("[Packager] -> Failed unpacking file {0} {1}", filePath, ex.what());
        }
    }

    void Packager::UnpackAndLoad(const String& filePath, const Vector<String>& filesToLoad, const wchar_t* pass, ResourceLoader* loader)
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

            // Progress callback.
            bit7z::BitExtractor*  pExtractor   = &extractor;
            ResourceProgressData* loadingData  = &ResourceManager::s_currentProgressData;
            loadingData->m_currentResourceName = "nofile";
            loadingData->m_currentProgress     = 0;
            loadingData->m_state               = ResourceProgressState::Pending;
            loadingData->m_progressTitle       = "Unpacking file " + filePath;

            extractor.setTotalCallback([=](uint64_t size) {
                loadingData->m_state = ResourceProgressState::Pending;
                pExtractor->setProgressCallback([size, loadingData](uint64_t processedSize) { loadingData->m_currentProgress = ((100.0f * (float)processedSize) / (float)size); });
            });

            extractor.setFileCallback([=](std::wstring file) {
                char* chr                          = Utility::WCharToChar(file.c_str());
                loadingData->m_currentResourceName = String(chr);
                LINA_INFO("[Packager] -> Unpacking: {0}", loadingData->m_currentResourceName);
                delete chr;
            });

            // Clear the current memory queue in the loader.
            MemoryQueue empty;
            loader->m_memoryResources.swap(empty);

            bit7z::BitArchiveInfo arc{lib, wdir, bit7z::BitFormat::SevenZip};

            auto items = arc.items();

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

                    if (pathStr.compare(file) == 0)
                    {
                        extractor.extract(wdir, stdv, item.index());

                        for (int i = 0; i < stdv.size(); i++)
                            v.push_back(stdv[i]);

                        loader->PushResourceFromMemory(file, v);

                        LINA_INFO("[Packager] -> Successfully loaded file {0} from the package: {1}", file, filePath);
                        loadedFiles++;
                        stdv.clear();
                        v.clear();
                        break;
                    }
                }
            }
        }
        catch (const bit7z::BitException& ex)
        {
            LINA_ERR("[Packager] -> Failed unpacking file {0} {1}", filePath, ex.what());
        }

        assert(loadedFiles == filesToLoad.size(), "Could not load all files!");
    }

} // namespace Lina::Resources
