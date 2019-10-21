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

out vec3 Normal;
out vec3 FragPos;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
	FragPos = vec3(model * vec4(position,1.0));
	Normal = mat3(inverseTransposeModel) * normal;
    texCoord0 = texCoord;
}

#elif defined(FS_BUILD)

layout (std140, row_major) uniform Lights
{
	vec4 ambientColor;
	vec4 cameraPosition;
	float ambientIntensity;
};

uniform sampler2D diffuse;
uniform vec3 objectColor;
uniform float specularIntensity;
uniform int specularExponent;

uniform vec3 lightPos;
uniform vec3 lightColor;

in vec3 Normal;
in vec3 FragPos;
out vec4 fragColor;

void main()
{
	vec3 ambient = vec3(ambientColor.x, ambientColor.y, ambientColor.z) * ambientIntensity;
	vec3 viewPos = vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z);
	
	//diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);  
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularExponent);
	vec3 specular = specularIntensity * spec * lightColor;  

	vec3 result = (ambient + diffuse + specular) * objectColor;	
	fragColor = vec4(result, 1.0);
}
#endif
