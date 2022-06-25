/*
Class: ShaderConstants

General purpose macros, used for defining variable names in shaders are stored here.

Timestamp: 2/16/2019 1:47:28 AM
*/

#pragma once

#ifndef ShaderConstants_HPP
#define ShaderConstants_HPP

#include <Data/String.hpp>

namespace Lina::Graphics
{

#define SC_LIGHTCOLOR              String(".color")
#define SC_LIGHTDISTANCE           String(".distance")
#define SC_LIGHTBIAS               String(".bias")
#define SC_LIGHTSHADOWNEAR         String(".shadowNear")
#define SC_LIGHTSHADOWFAR          String(".shadowFar")
#define SC_LIGHTCUTOFF             String(".cutOff")
#define SC_LIGHTOUTERCUTOFF        String(".outerCutOff")
#define SC_LIGHTINTENSITY          String(".intensity")
#define SC_LIGHTDIRECTION          String(".direction")
#define SC_LIGHTPOSITION           String(".position")
#define SC_DIRECTIONALLIGHT        String("directionalLight")
#define SC_DIRECTIONALLIGHT_EXISTS String("directionalLightExists")
#define SC_POINTLIGHTS             String("pointLights")
#define SC_SPOTLIGHTS              String("spotLights")

#define MAT_COLOR                            "material.color"
#define MAT_STARTCOLOR                       "material.startColor"
#define MAT_ENDCOLOR                         "material.endColor"
#define MAT_OBJECTCOLORPROPERTY              "material.objectColor"
#define MAT_SUNDIRECTION                     "material.sunDirection"
#define MAT_TEXTURE2D_DIFFUSE                "material.diffuse"
#define MAT_TEXTURE2D_ALBEDOMAP              "material.albedoMap"
#define MAT_TEXTURE2D_NORMALMAP              "material.normalMap"
#define MAT_TEXTURE2D_METALLICMAP            "material.metallicMap"
#define MAT_TEXTURE2D_AOMAP                  "material.aoMap"
#define MAT_TEXTURE2D_IRRADIANCEMAP          "material.irradianceMap"
#define MAT_TEXTURE2D_PREFILTERMAP           "material.prefilterMap"
#define MAT_TEXTURE2D_BRDFLUTMAP             "material.brdfLUTMap"
#define MAT_TEXTURE2D_SHADOWMAP              "material.shadowMap"
#define MAT_TEXTURE2D_ROUGHNESSMAP           "material.roughnessMap"
#define MAT_MAP_ENVIRONMENT                  "material.environmentMap"
#define MAT_MAP_REFLECTIONAREAMAP            "material.reflectionAreaMap"
#define MAT_MAP_SCREEN                       "material.screenMap"
#define MAT_MAP_PP                           "material.ppMap"
#define MAT_MAP_BLOOM                        "material.bloomMap"
#define MAT_MAP_OUTLINE                      "material.outlineMap"
#define MAT_MAP_EQUIRECTANGULAR              "material.equirectangularMap"
#define MAT_MAP_TARGETHDR                    "material.targetHDR"
#define MAT_MAP_GPOS                         "material.gPositionMap"
#define MAT_MAP_GNORMAL                      "material.gNormalMap"
#define MAT_MAP_GALBEDO                      "material.gAlbedoMap"
#define MAT_MAP_GEMISSION                    "material.gEmissionMap"
#define MAT_MAP_GMETALLICROUGHNESSAOWORKFLOW "material.gMetallicRoughnessAOWorkflowMap"
#define MAT_MAP_REFLECTION                   "material.reflectionMap"
#define MAT_MAP_SKYBOXIRR                    "material.skyboxIrradianceMap"
#define MAT_MAPS_SHADOWDEPTH                 "material.pointShadowDepthMaps"
#define MAT_SKYBOXIRRFACTOR                  "material.skyboxIrradianceFactor"
#define MAT_ENVIRONMENTRESOLUTION            "material.environmentResolution"
#define MAT_METALLICMULTIPLIER               "material.metallic"
#define MAT_ROUGHNESSMULTIPLIER              "material.roughness"
#define MAT_WORKFLOW                         "material.workflow"
#define MAT_SURFACETYPE                      "material.surfaceType"
#define MAT_TILING                           "material.tiling"
#define MAT_ISHORIZONTAL                     "material.horizontal"
#define MAT_BLOOMENABLED                     "material.bloomEnabled"
#define MAT_VIGNETTEENABLED                  "material.vignetteEnabled"
#define MAT_FXAAENABLED                      "material.fxaaEnabled"
#define MAT_FXAASPANMAX                      "material.fxaaSpanMax"
#define MAT_FXAAREDUCEMIN                    "material.fxaaReduceMin"
#define MAT_FXAAREDUCEMUL                    "material.fxaaReduceMul"
#define MAT_EXPOSURE                         "material.exposure"
#define MAT_GAMMA                            "material.gamma"
#define MAT_VIGNETTEAMOUNT                   "material.vignetteAmount"
#define MAT_VIGNETTEPOW                      "material.vignettePow"
#define MAT_INVERSESCREENMAPSIZE             "material.inverseScreenMapSize"
#define MAT_EXTENSION_TEXTURE2D              ".texture"
#define MAT_EXTENSION_ISACTIVE               ".isActive"
#define MAT_TIME                             "material.time"
#define MAT_CIRRUS                           "material.cirrus"
#define MAT_CUMULUS                          "material.cumulus"
#define UF_MATRIX_MODEL                      "model"
#define UF_MATRIX_VIEW                       "view"
#define UF_MATRIX_PROJECTION                 "projection"
#define UF_MVP                               "gMVP"
#define UF_FLOAT_TIME                        "uf_time"
#define UF_BONE_MATRICES                     "uf_boneMatrices"
#define UF_BOOL_SKINNED                      "uf_isSkinned"
#define UF_SHADOWMATRICES                    "uf_shadowMatrices"
#define UF_LIGHTPOS                          "uf_lightPos"
#define UF_LIGHTFARPLANE                     "uf_lightFarPlane"

} // namespace Lina::Graphics
#endif
