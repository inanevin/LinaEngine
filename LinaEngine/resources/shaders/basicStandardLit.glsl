/*
 * Copyright (C) 2019 Inan Evin - Thanks to the contributions of Benny Bobaganoosh
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
 
#include "common.glh"

varying vec2 texCoord0;

#if defined(VS_BUILD)
Layout(0) attribute vec3 position;
Layout(1) attribute vec2 texCoord;
Layout(2) attribute vec3 normal;
Layout(4) attribute mat4 model;
Layout(8) attribute mat4 inverseTransposeModel;

UB_GLOBAL

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
	FragPos = vec3(model * vec4(position,1.0));
	Normal = mat3(inverseTransposeModel) * normal;
    TexCoords = texCoord;
}

#elif defined(FS_BUILD)

#include "lighting.glh"

UB_GLOBAL
UB_GLOBAL_LIGHT
UB_GLOBAL_DEBUG

struct Material
{
sampler2D diffuse;
sampler2D specular;
vec3 objectColor;
float specularIntensity;
int specularExponent;
};

uniform Material material;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
out vec4 fragColor;


vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
	
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
	
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.specularExponent);
	
    // combine results
	vec3 ambient  = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * material.specularIntensity * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}


vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
	
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
	
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.specularExponent);
	
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
				 
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
	
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.specularExponent);
	
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));   
	
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
	
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

void main()
{
	vec3 viewPos = vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z);
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	
    vec3 result = CalculateDirectionalLight(directionalLight, norm, viewDir);
	
    // phase 2: point lights
     for(int i = 0; i < pointLightCount; i++)
        result += CalculatePointLight(pointLights[i], norm, FragPos, viewDir);    
		
    // phase 3: spot light
	for(int i = 0; i < spotLightCount; i++)
		result += CalculateSpotLight(spotLights[0], norm, FragPos, viewDir);    
    
	if(visualizeDepth)
		fragColor = vec4(vec3(gl_FragCoord.z), 1);
	else
		fragColor = vec4(result, 1.0);
}
#endif
