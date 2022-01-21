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
	MaterialSampler2D gPosition;
	MaterialSampler2D gNormal;
	MaterialSampler2D gAlbedoAO;
	MaterialSampler2D gMetallicRoughnessHDRI; // r metallic, g roughness, b HDRI.r, a HDRI.g
	MaterialSampler2D gF0HDRI; // rgb F0, a HDRI.b
	MaterialSamplerCube[MAX_POINT_LIGHT] pointShadowDepthMaps;
};
uniform Material material;

void main()
{
	vec3 gPos = texture(material.gPosition.texture, TexCoords).rgb;
	vec3 gNormal = texture(material.gNormal.texture, TexCoords).rgb;
	vec4 gAlbedoAO = texture(material.gAlbedoAO.texture, TexCoords);
	vec4 gMetallicRoughnessHDRI = texture(material.gMetallicRoughnessHDRI.texture, TexCoords);
	vec4 gF0HDRI = texture(material.gF0HDRI.texture, TexCoords);
	
	vec3 WorldPos = gPos.rgb;
 	vec3 albedo = gAlbedoAO.rgb;
  	float metallic = gMetallicRoughnessHDRI.r;
  	float roughness = gMetallicRoughnessHDRI.g;
  	float ao = gAlbedoAO.a;
  	vec3 N = gNormal.rgb;
  	vec3 V = normalize(vec3(LINA_CAMPOS.x, LINA_CAMPOS.y, LINA_CAMPOS.z) - WorldPos);
    vec3 Lo = vec3(0.0);
    vec3 F0 = gF0HDRI.rgb;
	vec3 HDRI = vec3(0);
	HDRI.r = gMetallicRoughnessHDRI.b;
	HDRI.g = gMetallicRoughnessHDRI.a;
	HDRI.b = gF0HDRI.a;
	
    // Directional Light
    if(directionalLightExists == 1)
	{
      vec3 L = -directionalLight.direction;
      vec3 radiance = directionalLight.color;
      Lo += CalculateLight(N, V, L, albedo, metallic, roughness, radiance, F0, 0.0);
    }

	int affectingLightCount = 0;
	
    // Point lights.
    for(int i = 0; i < LINA_PLIGHT_COUNT; ++i)
    {
        // calculate per-light radiance
        vec3 L = normalize(pointLights[i].position - WorldPos);
        float distance = length(pointLights[i].position - WorldPos);
		if(distance < pointLights[i].distance)
		{
			affectingLightCount++;
			float attenuation = 1.0 / (distance * distance);
			vec3 radiance = pointLights[i].color * attenuation;
			float shadow = material.pointShadowDepthMaps[i].isActive ? CalculateShadow(pointLights[i].shadowFar, pointLights[i].bias, WorldPos, pointLights[i].position, material.pointShadowDepthMaps[i].texture) : 0.0;
			Lo += CalculateLight(N, V, L, albedo, metallic, roughness, radiance, F0, shadow);			
		}		
    }

    // Spot lights
    for(int i = 0; i < LINA_SLIGHT_COUNT; ++i)
    {
		if(affectingLightCount >= MAX_LIGHT_PER_VERTEX)
			continue;
      // calculate per-light radiance
      vec3 L = normalize(spotLights[i].position - WorldPos);
      float distance = length(spotLights[i].position - WorldPos);
	  
	  if(distance < spotLights[i].distance)
	  {
		affectingLightCount++;
		float attenuation = 1.0 / (distance * distance);
		float theta = dot(L, normalize(-spotLights[i].direction));
		float epsilon = (spotLights[i].cutOff - spotLights[i].outerCutOff);
		float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);
		vec3 radiance = spotLights[i].color * attenuation * intensity;
		Lo += CalculateLight(N, V, L, albedo, metallic, roughness, radiance, F0, 0.0);
	  }  
    }
     
    vec3 color = HDRI + Lo;	
		
    // HDR tonemapping
	color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));

	fragColor = vec4(color, 1.0f);
}
#endif
