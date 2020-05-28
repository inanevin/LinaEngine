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

out VS_OUT
{
	vec3 Normal;
	vec3 FragPos;
	vec2 TexCoords;
} vs_out;


void main()
{
	vs_out.FragPos = vec3(model * vec4(position,1.0));
    vs_out.TexCoords = texCoord;
	vs_out.Normal = mat3(inverseTransposeModel) * normal;
    gl_Position = projection * view * model * vec4(position, 1.0);
}

#elif defined(FS_BUILD)

#include <../lightingData.glh>


layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;
layout (location = 2) out vec4 outlineColor;

struct MaterialSampler2D
{
	sampler2D texture;
	int isActive;
};

struct Material
{
MaterialSampler2D albedoMap;
MaterialSampler2D normalMap;
MaterialSampler2D metallicMap;
MaterialSampler2D roughnessMap;
MaterialSampler2D aoMap;
float metallicMultiplier;
float roughnessMultiplier;
vec2 tiling;
};

uniform Material material;


in VS_OUT
{
	vec3 Normal;
	vec3 FragPos;
	vec2 TexCoords;
} fs_in;

const float PI = 3.14159265359;

vec3 getNormalFromMap(vec2 uv)
{
	if(material.normalMap.isActive == 0)
		return normalize(fs_in.Normal);
    vec3 tangentNormal = texture(material.normalMap.texture, uv).xyz* 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.FragPos);
    vec3 Q2  = dFdy(fs_in.FragPos);
    vec2 st1 = dFdx(uv);
    vec2 st2 = dFdy(uv);

    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
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

void main()
{
	vec2 tiledTexCoords = vec2(fs_in.TexCoords.x * material.tiling.x, fs_in.TexCoords.y * material.tiling.y);

	vec3 camPos = vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z);
	vec3 albedo     = pow(material.albedoMap.isActive != 0  ? texture(material.albedoMap.texture, tiledTexCoords).rgb : vec3(1.0), vec3(2.2));
    float metallic  = material.metallicMap.isActive != 0 ? texture(material.metallicMap.texture, tiledTexCoords).r * material.metallicMultiplier : 1.0;
    float roughness = material.roughnessMap.isActive != 0 ?texture(material.roughnessMap.texture, tiledTexCoords).r * material.roughnessMultiplier : 1.0;
    float ao        = material.aoMap.isActive != 0 ? texture(material.aoMap.texture, tiledTexCoords).r : 1.0;

    vec3 N = getNormalFromMap(tiledTexCoords);
    vec3 V = normalize(camPos - fs_in.FragPos);

	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

	// reflectance equation
    vec3 Lo = vec3(0.0);
	for(int i = 0; i < pointLightCount; i++)
	{
		// calculate per-light radiance
        vec3 L = normalize(pointLights[i].position - fs_in.FragPos);
        vec3 H = normalize(V + L);
        float distance = length(pointLights[i].position - fs_in.FragPos);
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
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	}

	  // ambient lighting (note that the next IBL tutorial will replace
    // this ambient lighting with environment lighting).
    vec3 ambient = (vec3(0.03) * albedo * ao);
    vec3 color = ambient + Lo;


	// check whether fragment output is higher than threshold, if so output as brightness color
    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        brightColor = vec4(fragColor.rgb, 1.0);
    else
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);

  //outlineColor = vec4(0.0, 0.0, 0.0, 1.0);
	outlineColor = vec4(color, 1.0);

	fragColor = vec4(color, 1.0);
}
#endif
