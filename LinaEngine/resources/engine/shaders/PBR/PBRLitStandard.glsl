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
layout (location = 11) in mat4 inverseTransposeModel;

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
		
		WorldPos = vec3(model * totalPosition);
		Normal = mat3(model) * normal;
		gl_Position = VP *  vec4(WorldPos, 1.0f);
	}
	else
	{
		WorldPos = vec3(model * vec4(position, 1.0));
		Normal = mat3(model) * normal;
		gl_Position = VP * vec4(WorldPos, 1.0);
	}

}

#elif defined(FS_BUILD)
#include <../UniformBuffers.glh>
#include <../LightingData.glh>
#include <../MaterialSamplers.glh>
#include <../LightingCalculations.glh>

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

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
  MaterialSamplerCube[MAX_POINT_LIGHT] pointShadowDepthMaps;
  vec3 objectColor;
  float metallic;
  float roughness;
  int workflow;
  int surfaceType;
  vec2 tiling;
};

uniform Material material;
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

// ----------------------------------------------------------------------------
void main()
{
  vec2 tiled = vec2(TexCoords.x * material.tiling.x, TexCoords.y * material.tiling.y);
  // material properties
  vec3 albedo = material.albedoMap.isActive ? (pow(texture(material.albedoMap.texture, tiled).rgb, vec3(2.2)) * material.objectColor) : material.objectColor;
  float metallic = material.metallicMap.isActive ? (texture(material.metallicMap.texture,tiled).r * material.metallic) : material.metallic;
  float roughness = material.roughnessMap.isActive  ? (texture(material.roughnessMap.texture, tiled).r * material.roughness) : material.roughness;
  float ao = material.aoMap.isActive? texture(material.aoMap.texture, tiled).r : 1.0;

  vec3 N = material.normalMap.isActive ? getNormalFromMap(texture(material.normalMap.texture, tiled).rgb, tiled, WorldPos, Normal) : Normal;
  vec3 V = normalize(vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z) - WorldPos);


    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = material.workflow == 0 ? vec3(0.04) : albedo; // plastic 0, metallic 1
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    // Directional Light
    {
      vec3 L = -directionalLight.direction;
      vec3 radiance = directionalLight.color;
      Lo += CalculateLight(N, V, L, albedo, metallic, roughness, radiance, F0, 0.0);
    }

    // Point lights.
    for(int i = 0; i < pointLightCount; ++i)
    {
        // calculate per-light radiance
        vec3 L = normalize(pointLights[i].position - WorldPos);
        float distance = length(pointLights[i].position - WorldPos);

		if(distance < pointLights[i].distance)
		{
			float attenuation = 1.0 / (distance * distance);
			vec3 radiance = pointLights[i].color * attenuation;
			
			float shadow = material.pointShadowDepthMaps[i].isActive ? CalculateShadow(pointLights[i].shadowFar, pointLights[i].bias, WorldPos, pointLights[i].position, material.pointShadowDepthMaps[i].texture) : 0.0;
			Lo += CalculateLight(N, V, L, albedo, metallic, roughness, radiance, F0, shadow);			
		}		
    }

    // Spot lights
    for(int i = 0; i < spotLightCount; ++i)
    {
      // calculate per-light radiance
      vec3 L = normalize(spotLights[i].position - WorldPos);
      float distance = length(spotLights[i].position - WorldPos);
	  
	  if(distance < spotLights[i].distance)
	  {
		float attenuation = 1.0 / (distance * distance);

		float theta = dot(L, normalize(-spotLights[i].direction));
		float epsilon = (spotLights[i].cutOff - spotLights[i].outerCutOff);
		float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);
		vec3 radiance = spotLights[i].color * attenuation * intensity;

		Lo += CalculateLight(N, V, L, albedo, metallic, roughness, radiance, F0, 0.0);
	  }
      
    }

    vec3 ambient = vec3(0.0);

    bool irrActive = material.irradianceMap.isActive;
    bool preActive = material.prefilterMap.isActive;
    bool lutActive = material.brdfLUTMap.isActive;

    if(irrActive && preActive && lutActive)
    {
      vec3 R = reflect(-V, N);

      // ambient lighting (we now use IBL as the ambient term)
      vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
      vec3 kS = F;
      vec3 kD = 1.0 - kS;
      kD *= 1.0 - metallic;
	  
      vec3 irradiance = texture(material.irradianceMap.texture, N).rgb;
      vec3 diffuse = irradiance * albedo;

      // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
      const float MAX_REFLECTION_LOD = 4.0;
      vec3 prefilteredColor = textureLod(material.prefilterMap.texture, R,  roughness * MAX_REFLECTION_LOD).rgb;
      vec2 brdf  = texture(material.brdfLUTMap.texture, vec2(max(dot(N, V), 0.0), roughness)).rg;
      vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

      ambient = (kD * (diffuse + specular)) * ao * ambientColor.xyz;
    }
    else    
	{
		// ambient lighting (we now use IBL as the ambient term)
		vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
		vec3 kS = F;
		vec3 kD = 1.0 - kS;
		kD *= 1.0 - metallic;
		
		ambient = (kD * (albedo)) * ao * ambientColor.xyz;
	}
	 

    vec3 color = ambient + Lo;
	
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        brightColor = vec4(color.rgb, 1.0);
    else
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
		
    // HDR tonemapping
	// color = color / (color + vec3(1.0));
    // gamma correct
    //color = pow(color, vec3(1.0/2.2));

	float alpha =  material.surfaceType == 0 ? 1.0 : (material.albedoMap.isActive ? texture(material.albedoMap.texture, tiled).a : 1.0);
	fragColor = vec4(color, alpha);
}
#endif
