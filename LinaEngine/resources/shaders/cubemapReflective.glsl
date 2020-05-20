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
 
#include "common.glh"
#include <uniformBuffers.glh>
#include <utility.glh>


#if defined(VS_BUILD)
Layout(0) attribute vec3 position;
Layout(1) attribute vec2 texCoord;
Layout(2) attribute vec3 normal;
Layout(4) attribute mat4 model;
Layout(8) attribute mat4 inverseTransposeModel;


out vec3 Normal;
out vec3 Position;

void main()
{
	Normal = mat3(inverseTransposeModel) * normal;
	Position = vec3(model * vec4(position,1.0));
    gl_Position = projection * view * vec4(Position, 1.0);
}

#elif defined(FS_BUILD)

out vec4 fragColor;

in vec3 Normal;
in vec3 Position;
uniform samplerCube skybox;

uniform sampler2D screenTexture;

void main()
{
	vec3 camPos = vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z);
	vec3 I = normalize(Position - camPos);
    vec3 R = reflect(I, normalize(Normal));
    fragColor = vec4(texture(skybox, R).rgb, 1.0);
}
#endif
