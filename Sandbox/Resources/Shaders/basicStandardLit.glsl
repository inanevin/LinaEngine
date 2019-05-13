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
Layout(4) attribute mat4 transformMat;
Layout(8) attribute mat4 model;
Layout(12) attribute mat4 modelInverse;



out vec3 Normal;
out vec3 FragPos;

void main()
{
	FragPos = vec3(model * vec4(position,1.0));
    gl_Position = vec4(position, 1.0) * transformMat;
    texCoord0 = texCoord;
	Normal = mat3(modelInverse) * normal;
	
}

#elif defined(FS_BUILD)

uniform sampler2D diffuse;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 _ambientLightColor;
uniform float _ambientLightIntensity;
uniform vec3 _lightPos;
uniform vec3 _lightColor;
uniform vec3 _viewPos;

out vec4 FragColor;

void main()
{
	
    vec3 ambient = _ambientLightIntensity * _ambientLightColor;
	
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(_lightPos - FragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuseFactor = diff * _lightColor;
	vec3 result = (ambient + diffuseFactor);

	FragColor = texture2D(diffuse, texCoord0) * vec4(result, 1.0);
}
#endif
