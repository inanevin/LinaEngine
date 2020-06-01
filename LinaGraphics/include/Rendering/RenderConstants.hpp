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


#define SC_LIGHTCOLOR std::string(".color")
#define SC_LIGHTDISTANCE std::string(".distance")
#define SC_LIGHTCUTOFF std::string(".cutOff")
#define SC_LIGHTOUTERCUTOFF std::string(".outerCutOff")
#define SC_LIGHTINTENSITY std::string(".intensity")
#define SC_LIGHTDIRECTION std::string(".direction")
#define SC_LIGHTPOSITION std::string(".position")
#define SC_DIRECTIONALLIGHT std::string("directionalLight")
#define SC_POINTLIGHTS std::string("pointLights")
#define SC_SPOTLIGHTS std::string("spotLights")

#define MAT_COLOR "material.color"
#define MAT_STARTCOLOR "material.startColor"
#define MAT_ENDCOLOR "material.endColor"
#define MAT_OBJECTCOLORPROPERTY "material.objectColor"
#define MAT_SUNDIRECTION "material.sunDirection"
#define MAT_TEXTURE2D_DIFFUSE "material.diffuse"
#define MAT_TEXTURE2D_ALBEDOMAP "material.albedoMap"
#define MAT_TEXTURE2D_NORMALMAP "material.normalMap"
#define MAT_TEXTURE2D_METALLICMAP "material.metallicMap"
#define MAT_TEXTURE2D_AOMAP "material.aoMap"
#define MAT_TEXTURE2D_IRRADIANCEMAP "material.irradianceMap"
#define MAT_TEXTURE2D_PREFILTERMAP "material.prefilterMap"
#define MAT_TEXTURE2D_BRDFLUTMAP "material.brdfLUTMap"
#define MAT_TEXTURE2D_ROUGHNESSMAP "material.roughnessMap"
#define MAT_MAP_ENVIRONMENT "material.environmentMap"
#define MAT_MAP_SCREEN "material.screenMap"
#define MAT_MAP_BLOOM "material.bloomMap"
#define MAT_MAP_OUTLINE "material.outlineMap"
#define MAT_MAP_EQUIRECTANGULAR "material.equirectangularMap"
#define MAT_ENVIRONMENTRESOLUTION "material.environmentResolution"
#define MAT_METALLICMULTIPLIER "material.metallic"
#define MAT_ROUGHNESSMULTIPLIER "material.roughness"
#define MAT_WORKFLOW "material.workflow"
#define MAT_SURFACETYPE "material.surfaceType"
#define MAT_TILING "material.tiling"
#define MAT_ISHORIZONTAL "material.horizontal"
#define MAT_BLOOMENABLED "material.bloomEnabled"
#define MAT_FXAAENABLED "material.fxaaEnabled"
#define MAT_FXAASPANMAX "material.fxaaSpanMax"
#define MAT_FXAAREDUCEMIN "material.fxaaReduceMin"
#define MAT_FXAAREDUCEMUL "material.fxaaReduceMul"
#define MAT_EXPOSURE "material.exposure"
#define MAT_INVERSESCREENMAPSIZE "material.inverseScreenMapSize"
#define MAT_EXTENSION_TEXTURE2D ".texture"
#define MAT_EXTENSION_ISACTIVE ".isActive"
	
#define UF_MATRIX_MODEL "model"
#define UF_MATRIX_VIEW "view"
#define UF_MATRIX_PROJECTION "projection"

}
#endif