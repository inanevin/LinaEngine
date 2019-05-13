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
Layout(4) attribute mat4 transformMatrix;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMatrix;
uniform vec3 lightPos;

out vec3 Normal;
out vec3 FragPos;
out vec3 LightPos;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    FragPos = vec3(view * model * vec4(position, 1.0));
    texCoord0 = texCoord;
    Normal = mat3(normalMatrix) * normal;  	
	LightPos = vec3(view * vec4(lightPos, 1.0)); // Transform world-space light position to view-space light position

}

#elif defined(FS_BUILD)

uniform sampler2D diffuse;

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;

uniform vec3 ambientLightColor;
uniform float ambientLightIntensity;
uniform vec3 lightColor;

out vec4 FragColor;

void main()
{
	
    // ambient
    vec3 ambient = ambientLightIntensity * ambientLightColor;    
    
     // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseFactor = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(-FragPos); // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor; 
    
    vec3 result = (ambient + diffuseFactor + specular);
	
	FragColor = texture2D(diffuse, texCoord0) * vec4(result, 1.0);
}
#endif
