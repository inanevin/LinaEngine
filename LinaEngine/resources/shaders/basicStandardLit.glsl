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

layout (std140, row_major) uniform GlobalMatrices
{
	mat4 projection;
	mat4 view;
};

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

layout (std140, row_major) uniform Lights
{
	vec4 ambientColor;
	vec4 cameraPosition;
	float ambientIntensity;
};

struct Material
{
sampler2D diffuse;
sampler2D specular;
vec3 objectColor;
float specularIntensity;
int specularExponent;
};

uniform Material material;

struct Light
{
vec3 position;
vec3 color;
vec3 direction;
float constant;
float linear;
float quadratic;
float cutOff;
float outerCutOff;
};

uniform Light light;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
out vec4 fragColor;

void main()
{
	vec3 viewPos = vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z);
	vec3 ambient = (vec3(ambientColor.x, ambientColor.y, ambientColor.z) * ambientIntensity) * vec3(texture(material.diffuse, TexCoords));
	//diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light.color * vec3(texture(material.diffuse, TexCoords));

	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.specularExponent);
	vec3 specular = material.specularIntensity * spec * light.color * vec3(texture(material.specular, TexCoords));  
		
	// spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;
	
	// attenuation
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation;   
	
    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 1.0);
	
}
#endif
