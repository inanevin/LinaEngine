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

#include "Core/LinaAPI.hpp"
#include <string>


namespace LinaEngine::Graphics
{
	struct ShaderConstants
	{
		static LINAGRAPHICS_API std::string& standardLitShader;
		static LINAGRAPHICS_API std::string& standardUnlitShader;
		static LINAGRAPHICS_API std::string& skyboxCubemapShader;
		static LINAGRAPHICS_API std::string& skyboxGradientShader;
		static LINAGRAPHICS_API std::string& skyboxProceduralShader;
		static LINAGRAPHICS_API std::string& skyboxSingleColorShader;
	};

	struct MaterialConstants
	{
		static LINAGRAPHICS_API std::string& skyboxMaterialName;
		static LINAGRAPHICS_API std::string& colorProperty;
		static LINAGRAPHICS_API std::string& startColorProperty;
		static LINAGRAPHICS_API std::string& endColorProperty;
		static LINAGRAPHICS_API std::string& objectColorProperty;
		static LINAGRAPHICS_API std::string& sunDirectionProperty;
		static LINAGRAPHICS_API std::string& diffuseTextureProperty;
		static LINAGRAPHICS_API std::string& specularTextureProperty;
		static LINAGRAPHICS_API std::string& specularIntensityProperty;
		static LINAGRAPHICS_API std::string& specularExponentProperty;
	};
}
#endif