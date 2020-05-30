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

#define MC_SKYBOXMATERIALNAME "skybox_defaultMat"
#define MC_COLOR "material.color"
#define MC_STARTCOLOR "material.startColor"
#define MC_ENDCOLOR "material.endColor"
#define MC_OBJECTCOLORPROPERTY "material.objectColor"
#define MC_SUNDIRECTION "material.sunDirection"
#define MC_TEXTURE2D_DIFFUSE "material.diffuse"
#define MC_TEXTURE2D_ALBEDOMAP "material.albedoMap"
#define MC_TEXTURE2D_NORMALMAP "material.normalMap"
#define MC_TEXTURE2D_PARALLAXMAP "material.parallaxMap"
#define MC_TEXTURE2D_METALLICMAP "material.metallicMap"
#define MC_TEXTURE2D_AOMAP "material.aoMap"
#define MC_TEXTURE2D_IRRADIANCEMAP "material.irradianceMap"
#define MC_TEXTURE2D_PREFILTERMAP "material.prefilterMap"
#define MC_TEXTURE2D_BRDFLUTMAP "material.brdfLUTMap"
#define MC_TEXTURE2D_ROUGHNESSMAP "material.roughnessMap"
#define MC_TEXTURE2D_SPECULAR "material.specular"
#define MC_TEXTURE2D_SHADOWMAP "material.shadowMap"
#define MC_MAP_ENVIRONMENT "material.environmentMap"
#define MC_MAP_SCREEN "material.screenMap"
#define MC_MAP_BLOOM "bloomMap"
#define MC_MAP_OUTLINE "outlineMap"
#define MC_METALLICMULTIPLIER "material.metallic"
#define MC_ROUGHNESSMULTIPLIER "material.roughness"
#define MC_WORKFLOW "material.workflow"
#define MC_SPECULARINTENSIT "material.specularIntensity"
#define MC_SPECULAREXPONENT "material.specularExponent"
#define MC_OUTLINETHICKNESS "material.outlineThickness"
#define MC_SURFACETYPE "material.surfaceType"
#define MC_SPECULARACTIVE "material.specularActive"
#define MC_DIFFUSEACTIVE "material.diffuseActive"
#define MC_TILING "material.tiling"
#define MC_EXTENSION_TEXTURE2D ".texture"
#define MC_EXTENSION_ISACTIVE ".isActive"
#define MC_ISHORIZONTAL "horizontal"

#define MC_BLOOM "bloom"
#define MC_EXPOSURE "exposure"

#define UF_MATRIX_MODEL "model"
#define UF_MATRIX_VIEW "view"
#define UF_MATRIX_PROJECTION "projection"

#define UF_SKYBOXTEXTURE "skybox"
#define UF_TIME "time"

#define UF_LIGHTSPACEMATRIX "lightSpaceMatrix"
#define UF_POINTSHADOWS_SHADOWMATRICES "shadowMatrices"
#define UF_POINTSHADOWS_LIGHTPOS "lightPosition"
#define UF_POINTSHADOWS_FARPLANE "farPlane"
#define UF_MAP_EQUIRECTANGULAR "equirectangularMap"
#define UF_MAP_ENVIRONMENT "environmentMap"

#define MAT_LINASTENCILOUTLINE "lina_StencilOutline"

}
#endif