/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

/*
Class: ShaderConstants

General purpose macros, used for defining variable names in shaders are stored here.

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
#define SC_LIGHTBIAS std::string(".bias")
#define SC_LIGHTSHADOWNEAR std::string(".shadowNear")
#define SC_LIGHTSHADOWFAR std::string(".shadowFar")
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
#define MAT_TEXTURE2D_SHADOWMAP "material.shadowMap"
#define MAT_TEXTURE2D_ROUGHNESSMAP "material.roughnessMap"
#define MAT_MAP_ENVIRONMENT "material.environmentMap"
#define MAT_MAP_SCREEN "material.screenMap"
#define MAT_MAP_PP "material.ppMap"
#define MAT_MAP_BLOOM "material.bloomMap"
#define MAT_MAP_OUTLINE "material.outlineMap"
#define MAT_MAP_EQUIRECTANGULAR "material.equirectangularMap"
#define MAT_MAPS_SHADOWDEPTH "material.pointShadowDepthMaps"
#define MAT_ENVIRONMENTRESOLUTION "material.environmentResolution"
#define MAT_METALLICMULTIPLIER "material.metallic"
#define MAT_ROUGHNESSMULTIPLIER "material.roughness"
#define MAT_WORKFLOW "material.workflow"
#define MAT_SURFACETYPE "material.surfaceType"
#define MAT_TILING "material.tiling"
#define MAT_ISHORIZONTAL "material.horizontal"
#define MAT_BLOOMENABLED "material.bloomEnabled"
#define MAT_VIGNETTEENABLED "material.vignetteEnabled"
#define MAT_FXAAENABLED "material.fxaaEnabled"
#define MAT_FXAASPANMAX "material.fxaaSpanMax"
#define MAT_FXAAREDUCEMIN "material.fxaaReduceMin"
#define MAT_FXAAREDUCEMUL "material.fxaaReduceMul"
#define MAT_EXPOSURE "material.exposure"
#define MAT_GAMMA "material.gamma"
#define MAT_VIGNETTEAMOUNT "material.vignetteAmount"
#define MAT_VIGNETTEPOW "material.vignettePow"
#define MAT_INVERSESCREENMAPSIZE "material.inverseScreenMapSize"
#define MAT_EXTENSION_TEXTURE2D ".texture"
#define MAT_EXTENSION_ISACTIVE ".isActive"
#define MAT_TIME "material.time"
#define MAT_CIRRUS "material.cirrus"
#define MAT_CUMULUS "material.cumulus"
#define UF_MATRIX_MODEL "model"
#define UF_MATRIX_VIEW "view"
#define UF_MATRIX_PROJECTION "projection"
#define UF_MVP "gMVP"
#define UF_FLOAT_TIME "uf_time"
#define UF_BOOL_SKINNED "uf_isSkinned"
#define UF_SHADOWMATRICES "uf_shadowMatrices"
#define UF_LIGHTPOS "uf_lightPos"
#define UF_LIGHTFARPLANE "uf_lightFarPlane"

}
#endif