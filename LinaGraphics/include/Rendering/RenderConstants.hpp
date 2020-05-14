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

#define SC_STANDARDLITSHADER "shader_StandardLit"
#define SC_STANDARDUNLITSHADER "shader_StandardUnlit"
#define SC_SKYBOXCUBEMAPSHADER "shader_SkyboxCubemap"
#define SC_SKYBOXGRADIENTSHADER "shader_SkyboxGradient"
#define SC_SKYBOXPROCEDURALSHADER "shader_SkyboxProcedural"
#define SC_SKYBOXSINGLECOLORSHADER "shader_SkyboxSingleColor"

#define SC_LIGHTAMBIENT std::string(".ambient")
#define SC_LIGHTDIFFUSE std::string(".diffuse")
#define SC_LIGHTSPECULAR std::string(".specular")
#define SC_LIGHTCONSTANT std::string(".constant")
#define SC_LIGHTLINEAR std::string(".linear")
#define SC_LIGHTQUADRATIC std::string(".quadratic")
#define SC_LIGHTCUTOFF std::string(".cutOff")
#define SC_LIGHTOUTERCUTOFF std::string(".outerCutOff")
#define SC_LIGHTINTENSITY std::string(".intensity")
#define SC_LIGHTDIRECTION std::string(".direction")
#define SC_DIRECTIONALLIGHT std::string("directionalLight")
#define SC_POINTLIGHT std::string("pointLight")
#define SC_SPOTLIGHT std::string("spotLight")

#define MC_SKYBOXMATERIALNAME "skybox_defaultMat"
#define MC_COLORPROPERTY "material.color"
#define MC_STARTCOLORPROPERTY "material.startColor"
#define MC_ENDCOLORPROPERTY "material.endColor"
#define MC_OBJECTCOLORPROPERTY "material.objectColor"
#define MC_SUNDIRECTIONPROPERTY "material.sunDirection"
#define MC_DIFFUSETEXTUREPROPERTY "material.diffuse"
#define MC_SPECULARTEXTUREPROPERTY "material.specular"
#define MC_SPECULARINTENSITYPROPERTY "material.specularIntensity"
#define MC_SPECULAREXPONENTPROPERTY "material.specularExponent"


}
#endif