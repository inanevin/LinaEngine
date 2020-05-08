/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: ShaderConstants
Timestamp: 2/16/2019 1:47:28 AM

*/

#pragma once

#ifndef ShaderConstants_HPP
#define ShaderConstants_HPP

#include <string>

namespace LinaEngine::Graphics
{
	struct ShaderConstants
	{
		static std::string& standardLitShader;
		static std::string& standardUnlitShader;
		static std::string& skyboxCubemapShader;
		static std::string& skyboxGradientShader;
		static std::string& skyboxProceduralShader;
		static std::string& skyboxSingleColorShader;
	};

	struct MaterialConstants
	{
		static std::string& skyboxMaterialName;
		static std::string& colorProperty;
		static std::string& startColorProperty;
		static std::string& endColorProperty;
		static std::string& sunDirectionProperty;
		static std::string& diffuseTextureProperty;
	};
}
#endif