/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: EditorUtility
Timestamp: 5/9/2020 1:22:23 AM

*/
#pragma once

#ifndef EditorUtility_HPP
#define EditorUtility_HPP

#include <string>
#include <vector>
#include "Utility/Math/Vector.hpp"

namespace LinaEngine
{
	namespace Graphics
	{
		class Material;
	}
}

namespace LinaEditor
{
	class EditorUtility
	{

	public:

		EditorUtility();
		~EditorUtility();

		static void EditorUtility::ColorButton(LinaEngine::Vector4 col);
		static bool SelectableInput(const char* str_id, bool selected, int flags, char* buf, size_t buf_size);
		static bool CreateFolderInPath(const std::string& path);
		static bool GetDirectories(std::vector<std::string>& vec, const std::string& path);
		static bool DeleteDirectory(const std::string& path);
		static void SerializeMaterial(const std::string& path, class LinaEngine::Graphics::Material& mat);

	private:

	};
}


#endif
