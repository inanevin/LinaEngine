/*
 * Copyright (C) 2019 Inan Evin
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <../common.glh>
#include <../uniformBuffers.glh>
#include <../utility.glh>

#if defined(VS_BUILD)

Layout(0) attribute vec3 position;
Layout(1) attribute vec2 texCoord;
Layout(2) attribute vec3 normal;
Layout(3) attribute vec3 tangent;
Layout(4) attribute vec3 biTangent;
Layout(5) attribute mat4 model;
Layout(9) attribute mat4 inverseTransposeModel;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;


void main()
{
    TexCoords = texCoord;
    WorldPos = vec3(model * vec4(position, 1.0));
    Normal = mat3(model) * normal;

    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}

#elif defined(FS_BUILD)

#include <../lightingData.glh>
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;
layout (location = 2) out vec4 outlineColor;

struct MaterialSampler2D
{
	sampler2D texture;
	bool isActive;
};

struct MaterialSamplerCube
{
	samplerCube texture;
	bool isActive;
};

struct Material
{
  MaterialSampler2D albedoMap;
  MaterialSampler2D normalMap;
  MaterialSampler2D roughnessMap;
  MaterialSampler2D metallicMap;
  MaterialSampler2D aoMap;
  MaterialSampler2D brdfLUTMap;
  MaterialSamplerCube irradianceMap;
  MaterialSamplerCube prefilterMap;
  float metallic;
  float roughness;
  int workflow;
  vec2 tiling;
};

uniform Material material;
const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal
// mapping the usual way for performance anways; I do plan make a note of this
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap(vec3 normalColor, vec2 uv)
{
    vec3 tangentNormal = normalColor * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(uv);
    vec2 st2 = dFdy(uv);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
void main()
{
  vec2 tiled = vec2(TexCoords.x * material.tiling.x, TexCoords.y * material.tiling.y);
  // material properties
  vec3 albedo = material.albedoMap.isActive ? pow(texture(material.albedoMap.texture, tiled).rgb, vec3(2.2)) : vec3(1.0);
  float metallic = material.metallicMap.isActive ? texture(material.metallicMap.texture,tiled).r : material.metallic;
  float roughness = material.roughnessMap.isActive  ? texture(material.roughnessMap.texture, tiled).r : material.roughness;
  float ao = material.aoMap.isActive? texture(material.aoMap.texture, tiled).r : 1.0;

  vec3 N = material.normalMap.isActive ? getNormalFromMap(texture(material.normalMap.texture, tiled).rgb, tiled) : Normal;
  vec3 V = normalize(vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z) - WorldPos);


    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = material.workflow == 0 ? vec3(0.04) : albedo; // plastic 0, metallic 1
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i)
    {
        // calculate per-light radiance
        vec3 L = normalize(pointLights[i].position - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(pointLights[i].position - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = pointLights[i].color * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 nominator    = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;

         // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }



    vec3 ambient = vec3(1.0);
    if(material.irradianceMap.isActive  && material.prefilterMap.isActive && material.brdfLUTMap.isActive)
    {
      vec3 R = reflect(-V, N);

      // ambient lighting (we now use IBL as the ambient term)
      vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
      vec3 kS = F;
      vec3 kD = 1.0 - kS;
      kD *= 1.0 - metallic;

      vec3 irradiance = texture(material.irradianceMap.texture, N).rgb;
      vec3 diffuse      = irradiance * albedo;

      // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
      const float MAX_REFLECTION_LOD = 4.0;
      vec3 prefilteredColor = textureLod(material.prefilterMap.texture, R,  roughness * MAX_REFLECTION_LOD).rgb;
      vec2 brdf  = texture(material.brdfLUTMap.texture, vec2(max(dot(N, V), 0.0), roughness)).rg;
      vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

      ambient = (kD * diffuse + specular) * ao;
    }
    else
      ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));

    fragColor = vec4(color, 1.0);
}
#endif
