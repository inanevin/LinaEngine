/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: ResourceConstants
Timestamp: 1/5/2019 1:39:45 PM

*/

#include "LinaPch.hpp"
#include "Utility/ResourceConstants.hpp"

namespace LinaEngine
{

#ifdef LINA_PLATFORM_WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd

	std::string getResourcesPath()
	{
		char cCurrentPath[FILENAME_MAX];

		if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
		{
			return "";
		}

		cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */

		printf("The current working directory is %s", cCurrentPath);
	}


#endif



#define LINA_RESOURCESPATH "Resources\\"


	std::string& ResourceConstants::ShadersPath = std::string(LINA_RESOURCESPATH) + std::string("Shaders\\GLSL\\");
	std::string& ResourceConstants::TexturesPath = std::string(LINA_RESOURCESPATH) + std::string("Textures\\");
}

