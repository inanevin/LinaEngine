/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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
#include "Core/Log.hpp"
#include "Utility/FileUtility.hpp"
#include "Utility/EngineUtility.hpp"
#include "Core/ResourceBundle.hpp"
#include "bit7z/include/bitcompressor.hpp"
#include "bit7z/include/bitformat.hpp"
#include "bit7z/include/bitexception.hpp"
#include "bit7z/include/bitmemextractor.hpp"
#include "bit7z/include/bittypes.hpp"
#include "bit7z/include/bitextractor.hpp"

namespace Lina::Resources
{
	void Packager::PackageDirectory(const std::string& dir, const std::string& output, const wchar_t* pass, ResourceProgressData* loadingData)
	{
		try
		{
			// Delete first if exists.
			if (FileUtility::FileExists(output))
				FileUtility::DeleteFileInPath(output);

			// Setup compression.
			bit7z::Bit7zLibrary lib{ L"7z.dll" };
			bit7z::BitCompressor compressor{ lib, bit7z::BitFormat::SevenZip };
			compressor.setCompressionMethod(bit7z::BitCompressionMethod::Lzma);
			compressor.setPassword(pass);

			// Progress callback.
			bit7z::BitCompressor* pCompressor = &compressor;
			loadingData->m_currentResourceName = "Packing directory";
			loadingData->m_currentProgress = 0;
			loadingData->m_state = ResourceProgressState::Pending;
			loadingData->m_progressTitle = "Packing directory" + std::string(dir);

			compressor.setTotalCallback([=](uint64_t size) {
				loadingData->m_state = ResourceProgressState::Pending;
				pCompressor->setProgressCallback([size, loadingData](uint64_t processedSize)
					{loadingData->m_currentProgress = static_cast<int>((100.0 * processedSize) / size); });
				});

			compressor.setFileCallback([=](std::wstring file)
				{
					char* chr = Utility::WCharToChar(file.c_str());
					loadingData->m_currentResourceName = std::string(chr);
					delete chr;
				});

			// Setup path.
			const char* dirchr = dir.c_str();
			const char* outputchr = output.c_str();
			const size_t dirSize = strlen(dirchr);
			const size_t outputSize = strlen(outputchr);
			std::wstring wdir(dirSize, L'#');
			std::wstring woutput(outputSize, L'#');
			mbstowcs(&wdir[0], dirchr, dirSize);
			mbstowcs(&woutput[0], outputchr, outputSize);

			// compress.
			compressor.compressDirectory(wdir, woutput);

			LINA_TRACE("[Packager] -> Successfully packed directory {0}", dir);
		}
		catch (const bit7z::BitException& ex)
		{
			LINA_ERR("[Packager] -> Failed packaging directory {0} {1}", dir, ex.what());
		}
	}

	void Packager::PackageFileset(std::vector<std::string> files, const std::string& output, const wchar_t* pass, ResourceProgressData* loadingData)
	{
		try
		{

			if (files.empty())
			{
				LINA_TRACE("[Packager] -> No files found in fileset, aborting packing...");
				return;
			}
			// Delete first if exists.
			if (FileUtility::FileExists(output))
				FileUtility::DeleteFileInPath(output);

			// Setup compression.
			bit7z::Bit7zLibrary lib{ L"7z.dll" };
			bit7z::BitCompressor compressor{ lib, bit7z::BitFormat::SevenZip };
			compressor.setCompressionMethod(bit7z::BitCompressionMethod::Lzma);
			compressor.setPassword(pass);

			// Progress callback.
			bit7z::BitCompressor* pCompressor = &compressor;
			loadingData->m_currentResourceName = "Packing directory";
			loadingData->m_currentProgress = 0;
			loadingData->m_state = ResourceProgressState::Pending;
			loadingData->m_progressTitle = "Packing multiple files...";

			compressor.setTotalCallback([=](uint64_t size) {
				loadingData->m_state = ResourceProgressState::Pending;
				pCompressor->setProgressCallback([size, loadingData](uint64_t processedSize)
					{loadingData->m_currentProgress = static_cast<int>((100.0 * processedSize) / size); });
				});

			compressor.setFileCallback([=](std::wstring file)
				{
					char* chr = Utility::WCharToChar(file.c_str());
					loadingData->m_currentResourceName = std::string(chr);
					delete chr;
				});

			// Create a vector of unicode file paths.
			std::vector<std::wstring> wfiles;
			for (auto& file : files)
			{
				const size_t size = strlen(file.c_str());
				std::wstring wfile(size, L'#');
				mbstowcs(&wfile[0], file.c_str(), size);
				wfiles.push_back(wfile);
			}

			// Setup output path.
			const char* outputchr = output.c_str();
			const size_t outSize = strlen(outputchr);
			std::wstring woutput(outSize, L'#');
			mbstowcs(&woutput[0], outputchr, outSize);

			// compress.
			compressor.compress(wfiles, woutput);
			LINA_TRACE("[Packager] -> Successfully packed files starting with: {0}", files[0]);
		}
		catch (const bit7z::BitException& ex)
		{
			LINA_ERR("[Packager] -> Failed packaging files {0} {1}", files[0], ex.what());
		}
	}

	void Packager::Unpack(const std::string& filePath, const wchar_t* pass, ResourceBundle* outBundle, ResourceProgressData* loadingData, std::unordered_map<std::string, ResourceType>& unpackedResources)
	{
		try {

			// Delete first if exists.
			if (!FileUtility::FileExists(filePath))
			{
				LINA_ERR("[Packager] -> Failed unpacking file, file does not exist: {0}", filePath);
				return;
			}

			// Setup extractor.
			bit7z::Bit7zLibrary lib{ L"7z.dll" };
			bit7z::BitExtractor extractor{ lib, bit7z::BitFormat::SevenZip };
			extractor.setPassword(pass);

			// Setup path.
			const char* filePathChr = filePath.c_str();
			const size_t dirSize = strlen(filePathChr);
			std::wstring wdir(dirSize, L'#');
			mbstowcs(&wdir[0], filePathChr, dirSize);

			// Progress callback.
			bit7z::BitExtractor* pExtractor = &extractor;
			loadingData->m_currentResourceName = "nofile";
			loadingData->m_currentProgress = 0;
			loadingData->m_state = ResourceProgressState::Pending;
			loadingData->m_progressTitle = "Unpacking file " + filePath;

			extractor.setTotalCallback([=](uint64_t size) {
				loadingData->m_state = ResourceProgressState::Pending;
				pExtractor->setProgressCallback([size, loadingData](uint64_t processedSize)
					{loadingData->m_currentProgress = static_cast<int>((100.0 * processedSize) / size); });
				});

			extractor.setFileCallback([=](std::wstring file)
				{
					char* chr = Utility::WCharToChar(file.c_str());
					loadingData->m_currentResourceName = std::string(chr);
					delete chr;
				});

			// Extract.
			std::map <std::wstring, std::vector<bit7z::byte_t>> map;
			extractor.extract(wdir, map);

			// Sort the resources into their respective packages in the bundle.
			for (auto& item : map)
			{
				// Setup paths
				const char* filePath = Utility::WCharToChar(item.first.c_str());
				std::string filePathStr = filePath;
				std::replace(filePathStr.begin(), filePathStr.end(), '\\', '/');
				std::string ext = FileUtility::GetFileExtension(filePath);

				// Fill the reference table to be used after unpacking.
				ResourceType resType = GetResourceType(ext);
				unpackedResources[filePathStr] = resType;

				// Pass the resource to bundle.
				outBundle->m_rawPackages[resType][StringID(filePathStr.c_str())] = item.second;
			}

			LINA_TRACE("[Packager] -> Successfully unpacked file {0}", filePath);
		}
		catch (const bit7z::BitException& ex) {
			LINA_ERR("[Packager] -> Failed unpacking file {0} {1}", filePath, ex.what());
		}
	}

}