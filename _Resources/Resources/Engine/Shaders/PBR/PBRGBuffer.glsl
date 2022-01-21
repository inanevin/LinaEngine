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


#if defined(VS_BUILD)
#include <../UniformBuffers.glh>
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 biTangent;
layout (location = 5) in ivec4 boneIDs;
layout (location = 6) in vec4 boneWeights;
layout (location = 7) in mat4 model;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;


uniform bool uf_isSkinned;
const int MAX_BONES = 150;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 uf_boneMatrices[MAX_BONES];

  
void main()
{
	TexCoords = texCoords;	
	
	if(uf_isSkinned)
	{
		
		vec4 totalPosition = vec4(0.0f);
		
		for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
		{
			if(boneIDs[i] == -1) 
				continue;
				
			if(boneIDs[i] >= MAX_BONES) 
			{
				totalPosition = vec4(position,1.0f);
				break;
			}
			
			vec4 localPosition = uf_boneMatrices[boneIDs[i]] * vec4(position,1.0f);
			totalPosition += localPosition * boneWeights[i];
			vec3 localNormal = mat3(uf_boneMatrices[boneIDs[i]]) * normal;
		}
		
		//mat4 BoneTransform = uf_boneMatrices[boneIDs[0]] * boneWeights[0];
		//BoneTransform += uf_boneMatrices[boneIDs[1]] * boneWeights[1];
		//BoneTransform += uf_boneMatrices[boneIDs[2]] * boneWeights[2];
		//BoneTransform += uf_boneMatrices[boneIDs[3]] * boneWeights[3];
		
		WorldPos = vec3(model * vec4(position,1.0f));
		Normal = mat3(model) * normal;
		gl_Position = LINA_VP *  vec4(WorldPos, 1.0f);
	}
	else
	{
		WorldPos = vec3(model * vec4(position, 1.0));
		Normal = mat3(model) * normal;
		gl_Position = LINA_VP * vec4(WorldPos, 1.0);
	}

}

#elif defined(FS_BUILD)
#include <../UniformBuffers.glh>
#include <../LightingData.glh>
#include <../MaterialSamplers.glh>
#include <../LightingCalculations.glh>

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoAO;
layout (location = 3) out vec4 gMetallicRoughnessHDRI; // r metallic, g roughness, b HDRI.r, a HDRI.g
layout (location = 4) out vec4 gF0HDRI; // rgb F0, a HDRI.b

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

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
  vec3 objectColor;
  float metallic;
  float roughness;
  int workflow;
  int surfaceType;
  vec2 tiling;
};

uniform Material material;


// ----------------------------------------------------------------------------
void main()
{
	vec2 tiled = vec2(TexCoords.x * material.tiling.x, TexCoords.y * material.tiling.y);
 	vec3 albedo = material.albedoMap.isActive ? (pow(texture(material.albedoMap.texture, tiled).rgb, vec3(2.2)) * material.objectColor) : 
 	pow(material.objectColor, vec3(2.2));
  	float metallic = material.metallicMap.isActive ? (texture(material.metallicMap.texture,tiled).r * material.metallic) : material.metallic;
  	float roughness = material.roughnessMap.isActive  ? (texture(material.roughnessMap.texture, tiled).r * material.roughness) : material.roughness;
  	float ao = material.aoMap.isActive? texture(material.aoMap.texture, tiled).r : 1.0;
  	vec3 N = material.normalMap.isActive ? getNormalFromMap(texture(material.normalMap.texture, tiled).rgb, tiled, WorldPos, Normal) : Normal;
	vec3 V = normalize(vec3(LINA_CAMPOS.x, LINA_CAMPOS.y, LINA_CAMPOS.z) - WorldPos);

	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = material.workflow == 0 ? vec3(0.04) : albedo; // plastic 0, metallic 1
    F0 = mix(F0, albedo, metallic);
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
	
    vec3 R = reflect(-V, N);    
    vec3 irradiance = texture(material.irradianceMap.texture, N).rgb;
    vec3 diffuse = irradiance * albedo;
      
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(material.prefilterMap.texture, R,  roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf  = texture(material.brdfLUTMap.texture, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
	vec3 hdri =(kD * diffuse + specular) * ao * LINA_AMBIENT.xyz; 
    
	gPosition = WorldPos;
    gNormal = normalize(N);
    gAlbedoAO.rgb = albedo;
	gAlbedoAO.a = ao;
	gMetallicRoughnessHDRI.r = metallic;
	gMetallicRoughnessHDRI.g = roughness;
	gMetallicRoughnessHDRI.b = hdri.r;
	gMetallicRoughnessHDRI.a = hdri.g;
	gF0HDRI.xyz = F0.xyz;
	gF0HDRI.a = hdri.b;
}
#endif




