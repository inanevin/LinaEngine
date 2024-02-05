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

#ifndef FileData_HPP
#define FileData_HPP

#include "Common/StringID.hpp"
#include "Common/Data/String.hpp"
#include "Common/Data/Vector.hpp"
#include <filesystem>

namespace Lina
{
	struct Folder;

	struct DirectoryItem
	{
		DirectoryItem()	 = default;
		~DirectoryItem() = default;

		String							fullPath   = "";
		String							name	   = ""; // name without extension
		Folder*							parent	   = nullptr;
		TypeID							typeID	   = 0;
		StringID						sid		   = 0;
		bool							isRenaming = false;
		std::filesystem::file_time_type lastWriteTime;
	};

	struct File : public DirectoryItem
	{
		File()	= default;
		~File() = default;

		String folderPath = "";
		String extension  = "";
		String fullName	  = ""; // name with extension
	};

	struct Folder : public DirectoryItem
	{
		Folder() = default;
		~Folder();

		Vector<File*>	files;
		Vector<Folder*> folders;
		bool			isOpen = false;
	};
} // namespace Lina

#endif
