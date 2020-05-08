/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: RenderEngine
Timestamp: 4/27/2019 11:18:07 PM

*/

#include "LinaPch.hpp"
#include "Rendering/RenderConstants.hpp"
namespace LinaEngine::Graphics
{
	std::string& ShaderConstants::standardLitShader = std::string("shader_StandardLit");
	std::string& ShaderConstants::standardUnlitShader = std::string("shader_StandardUnlit");
	std::string& ShaderConstants::skyboxCubemapShader = std::string("shader_SkyboxCubemap");
	std::string& ShaderConstants::skyboxGradientShader = std::string("shader_SkyboxGradient");
	std::string& ShaderConstants::skyboxProceduralShader = std::string("shader_SkyboxProcedural");
	std::string& ShaderConstants::skyboxSingleColorShader = std::string("shader_SkyboxSingleColor");

	std::string& MaterialConstants::skyboxMaterialName = std::string("skybox_defaultMat");
	std::string& MaterialConstants::colorProperty = std::string("material.color");
	std::string& MaterialConstants::startColorProperty = std::string("material.startColor");
	std::string& MaterialConstants::endColorProperty = std::string("material.endColor");
	std::string& MaterialConstants::sunDirectionProperty = std::string("material.sunDirection");
	std::string& MaterialConstants::diffuseTextureProperty = std::string("material.diffuse");
}