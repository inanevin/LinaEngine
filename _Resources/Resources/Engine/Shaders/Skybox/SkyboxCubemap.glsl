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
#include <SkyboxCommon.glh>
out vec3 WorldPos;

	
void main()
{
	mat4 inverseProjection = inverse(LINA_PROJ);
    mat3 inverseModelview = transpose(mat3(LINA_VIEW));
    vec3 unprojected = (inverseProjection * vec4(data[gl_VertexID], 0.0, 1.0)).xyz;
    vec3 eyeDirection = inverseModelview * unprojected;
    gl_Position = vec4(data[gl_VertexID], 0.0, 1.0);
    WorldPos = eyeDirection;
}

#elif defined(FS_BUILD)
#include <../MaterialSamplers.glh>
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;
in vec3 WorldPos;
struct Material
{
  MaterialSamplerCube environmentMap;
};
uniform Material material;

void main()
{
   fragColor = material.environmentMap.isActive ? texture(material.environmentMap.texture, WorldPos) : vec4(1.0);
   
   float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0)
		brightColor = vec4(fragColor.rgb, 1.0);
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
#endif
