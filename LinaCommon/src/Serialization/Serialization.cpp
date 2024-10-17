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

#include "Common/Serialization/Serialization.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Serialization/Compressor.hpp"
#include "Common/Serialization/CommonSerialization.hpp"

namespace Lina
{
	bool Serialization::WriteToFile(const String& fileInput, const String& targetFilePath)
	{
		std::ofstream outFile(targetFilePath.c_str());

		if (outFile.is_open())
		{
			outFile << fileInput;
			outFile.close();
		}
		else
		{
			LINA_ERR("Failed writing to file! {0}", targetFilePath);
			return false;
		}

		return true;
	}

	bool Serialization::SaveToFile(const char* path, OStream& stream)
	{
		if (FileSystem::FileOrPathExists(path))
			FileSystem::DeleteFileInPath(path);

		std::ofstream wf(path, std::ios::out | std::ios::binary);

		if (!wf)
		{
			LINA_ERR("[Serialization] -> Could not open file for writing! {0}", path);
			return false;
		}

		OStream compressed = Compressor::Compress(stream);
		compressed.WriteToOFStream(wf);
		wf.close();
		compressed.Destroy();

		if (!wf.good())
		{
			LINA_ERR("[Serialization] -> Error occured while writing the file! {0}", path);
			return false;
		}

		return true;
	}

	IStream Serialization::LoadFromFile(const char* path)
	{
		if (!FileSystem::FileOrPathExists(path))
		{
			LINA_ERR("[Serialization] -> File doesn't exists: {0}", path);
			return {};
		}

		std::ifstream rf(path, std::ios::out | std::ios::binary);

		if (!rf)
		{
			LINA_ERR("[Serialization] -> Could not open file for reading! {0}", path);
			return IStream();
		}

		auto size = std::filesystem::file_size(path);

		// Create
		IStream readStream;
		readStream.Create(size);
		readStream.ReadFromIFStream(rf);
		rf.close();

		if (!rf.good())
		{
			LINA_ERR("[Serialization] -> Error occured while reading the file! {0}", path);
			readStream.Destroy();
			return {};
		}

		IStream decompressedStream = Compressor::Decompress(readStream);
		readStream.Destroy();
		return decompressedStream;
	}

} // namespace Lina
