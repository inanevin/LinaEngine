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
	MaterialSampler2D gPositionMetallicMap;	
	MaterialSampler2D gNormalRoughnessMap;	
	MaterialSampler2D gAlbedoAOMap;		
	MaterialSampler2D gEmissionWorkflowMap;
	MaterialSampler2D brdfLUTMap;
    MaterialSamplerCube irradianceMap;
    MaterialSamplerCube prefilterMap;
	MaterialSamplerCube[MAX_POINT_LIGHT] pointShadowDepthMaps;
};

uniform Material material;

void main()
{
	vec4 gPosMet = material.gPositionMetallicMap.isActive ? texture(material.gPositionMetallicMap.texture, TexCoords) : vec4(0.0);
	vec4 gNormRough= material.gNormalRoughnessMap.isActive ? texture(material.gNormalRoughnessMap.texture, TexCoords) : vec4(0.0);
	vec4 gAlbAO = material.gAlbedoAOMap.isActive ? texture(material.gAlbedoAOMap.texture, TexCoords) : vec4(0.0);
	vec4 gEmWf = material.gEmissionWorkflowMap.isActive ? texture(material.gEmissionWorkflowMap.texture, TexCoords) : vec4(0.0);
	
	vec3 WorldPos = gPosMet.rgb;
 	vec3 albedo = gAlbAO.rgb;
	vec3 emission = gEmWf.rgb;
	float workflow = gEmWf.a;
  	float metallic = gPosMet.a;
  	float roughness = gNormRough.a;
  	float ao = gAlbAO.a;
  	vec3 N = gNormRough.rgb;
  	vec3 V = normalize(vec3(LINA_CAMPOS.x, LINA_CAMPOS.y, LINA_CAMPOS.z) - WorldPos);
    vec3 Lo = vec3(0.0);
	vec3 finalColor = vec3(0.0);
	vec3 ambientMultiplier = vec3(1.0);
	
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
			ambientMultiplier = LINA_AMBIENT.xyz;
	}
	else
		finalColor = albedo;
     
    finalColor += Lo * ambientMultiplier;
	
    // HDR tonemapping
	//color = color / (color + vec3(1.0));
    // gamma correct
   // color = pow(color, vec3(1.0/2.2));

	fragColor = vec4(finalColor, 1.0f);
}
#endif


