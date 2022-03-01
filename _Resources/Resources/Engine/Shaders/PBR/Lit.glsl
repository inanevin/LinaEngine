/*
*	This file is a part of: Lina Engine
*	https://github.com/inanevin/LinaEngine
*	
*	Author: Inan Evin
*	http://www.inanevin.com
*	
*	Copyright (c) [2018-] [Inan Evin]
*	
*	Permission is hereby granted, free of charge, to any person obtaining a copy
*	of this software and associated documentation files (the "Software"), to deal
*	in the Software without restriction, including without limitation the rights
*	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*	copies of the Software, and to permit persons to whom the Software is
*	furnished to do so, subject to the following conditions:
*	
*	The above copyright notice and this permission notice shall be included in all
*	copies or substantial portions of the Software.
*	
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*	SOFTWARE.
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

layout (location = 0) out vec4 gPosition;		// rgb = position
layout (location = 1) out vec4 gNormal;			// rgb = normal
layout (location = 2) out vec4 gAlbedo;					
layout (location = 3) out vec4 gEmission;		    				// rgb = emission, a = workflow
layout (location = 4) out vec4 gMetallicRoughnessAOWorkflow;		// r = metallic, g = roughness, b = ao, a = workflow



in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

struct Material
{
  MaterialSampler2D albedoMap;
  MaterialSampler2D normalMap;
  MaterialSampler2D metallicRoughnessAOMap;
  MaterialSampler2D emissiveMap;
  vec4 objectColor;
  float metallic;
  float roughness;
  float emissionIntensity;
  int workflow;
  int surfaceType;
  vec2 tiling;
};

uniform Material material;


// ----------------------------------------------------------------------------
void main()
{
	vec2 tiled = vec2(TexCoords.x * material.tiling.x, TexCoords.y * material.tiling.y);
 	vec4 albedo = material.albedoMap.isActive ? texture(material.albedoMap.texture, tiled).rgba * material.objectColor : 
 	material.objectColor;
	vec3 emission = material.emissiveMap.isActive ? texture(material.emissiveMap.texture, tiled).rgb : vec3(0.0);
	float alpha = material.surfaceType == 0 ? 1.0 : albedo.a;

	bool metRoughAOActive = material.metallicRoughnessAOMap.isActive;
  	float metallic = metRoughAOActive ? (texture(material.metallicRoughnessAOMap.texture,tiled).r * material.metallic) : material.metallic;
  	float roughness = metRoughAOActive  ? (texture(material.metallicRoughnessAOMap.texture, tiled).g * material.roughness) : material.roughness;
  	float ao = metRoughAOActive ? texture(material.metallicRoughnessAOMap.texture, tiled).b : 1.0;
  	vec3 N = material.normalMap.isActive ? getNormalFromMap(texture(material.normalMap.texture, tiled).rgb, tiled, WorldPos, Normal) : Normal;
	N = normalize(N);
	
	gPosition = vec4(WorldPos, 1.0f);
	gNormal = vec4(N, 1.0f);
	gAlbedo = vec4(albedo.rgb, alpha);
	gEmission = vec4(emission * material.emissionIntensity, 1.0f);
	gMetallicRoughnessAOWorkflow = vec4(metallic, roughness, ao, float(material.workflow));
}
#endif






