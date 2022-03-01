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
out vec2 TexCoords;

void main()
{
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    TexCoords = texCoords;
}

#elif defined(FS_BUILD)
#include <../UniformBuffers.glh>
#include <../LightingData.glh>
#include <../MaterialSamplers.glh>
#include <../LightingCalculations.glh>

out vec4 fragColor;
in vec2 TexCoords;

struct Material
{
	MaterialSampler2D gPositionMap;	
	MaterialSampler2D gNormalMap;	
	MaterialSampler2D gAlbedoMap;		
	MaterialSampler2D gEmissionMap;
	MaterialSampler2D gMetallicRoughnessAOWorkflowMap;
	MaterialSampler2D brdfLUTMap;
	MaterialSamplerCube reflectionMap;
	MaterialSamplerCube skyboxIrradianceMap;
    MaterialSamplerCube irradianceMap;
    MaterialSamplerCube prefilterMap;
	MaterialSamplerCube[MAX_POINT_LIGHT] pointShadowDepthMaps;
	float skyboxIrradianceFactor;
};

uniform Material material;

void main()
{
	vec4 gPos = material.gPositionMap.isActive ? texture(material.gPositionMap.texture, TexCoords) : vec4(0.0);
	vec4 gNorm = material.gNormalMap.isActive ? texture(material.gNormalMap.texture, TexCoords) : vec4(0.0);
	vec4 gAlb = material.gAlbedoMap.isActive ? texture(material.gAlbedoMap.texture, TexCoords) : vec4(0.0);
	vec4 gEmission = material.gEmissionMap.isActive ? texture(material.gEmissionMap.texture, TexCoords) : vec4(0.0);
	vec4 gMetRoughAOWf = material.gMetallicRoughnessAOWorkflowMap.isActive ? texture(material.gMetallicRoughnessAOWorkflowMap.texture, TexCoords) : vec4(0.0);	
	vec3 WorldPos = gPos.rgb;
 	vec3 albedo = gAlb.rgb;
	vec3 emission = gEmission.rgb;
  	float metallic = gMetRoughAOWf.r;
  	float roughness = gMetRoughAOWf.g;
  	float ao = gMetRoughAOWf.b;
	float workflow = gMetRoughAOWf.a;
  	vec3 N = gNorm.rgb;
  	vec3 V = normalize(vec3(LINA_CAMPOS.x, LINA_CAMPOS.y, LINA_CAMPOS.z) - WorldPos);
    vec3 Lo = vec3(0.0);
    vec3 R = reflect(-V, normalize(N));
	vec3 finalColor = vec3(0.0);
	vec3 ambientMultiplier = vec3(1.0);
	vec4 reflection = material.reflectionMap.isActive ? texture(material.reflectionMap.texture, N) : vec4(0.0f);
	vec3 skyboxIrr = material.skyboxIrradianceMap.isActive ? texture(material.skyboxIrradianceMap.texture, R).rgb * material.skyboxIrradianceFactor : vec3(0.0f);
	
	// IF LIT WORKFLOW
	if(workflow != 2.0f)
	{
		vec3 F0 = workflow == 0.0f ? vec3(0.04) : albedo;
		F0 = mix(F0, albedo, metallic);
	
		// Directional Light
		if(directionalLightExists == 1)
		{
			vec3 L = -directionalLight.direction;
			vec3 radiance = directionalLight.color;
			Lo += CalculateLight(N, V, L, albedo, metallic, roughness, radiance, F0, 0.0);
		}

		// Point lights.
		for(int i = 0; i < LINA_PLIGHT_COUNT; ++i)
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
		for(int i = 0; i < LINA_SLIGHT_COUNT; ++i)
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
		
		bool prefilterActive = material.prefilterMap.isActive;
		bool brdfActive = material.brdfLUTMap.isActive;
		bool irradianceActive = material.irradianceMap.isActive;
		
		// IF WE HAVE AN HDRI ENVIRONMENT MAP & HDRI SKYBOX TO SUPPORT IT
		if(prefilterActive && brdfActive && irradianceActive)
		{
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
			finalColor = (kD * diffuse + specular) * ao * LINA_AMBIENT.xyz; 
		}
		else
		{
			ambientMultiplier = LINA_AMBIENT.xyz;
			finalColor = skyboxIrr;
		}
	}
	else
	{
		// IF UNLIT WORKFLOW
		finalColor = albedo;
	}
     
    finalColor += Lo * ambientMultiplier + emission + reflection.rgb;
	
    // HDR tonemapping
	//color = color / (color + vec3(1.0));
    // gamma correct
   // color = pow(color, vec3(1.0/2.2));

	fragColor = vec4(finalColor, 1.0f);
}
#endif


