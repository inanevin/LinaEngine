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

out vec2 TexCoords;

uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    TexCoords = texCoord;
}

#elif defined(FS_BUILD)

struct Material
{
	sampler2D diffuse;
};

uniform Material material;

in vec2 TexCoords;
out vec4 fragColor;

void main()
{
	if(visualizeDepth)
	{
		float depth = LinearizeDepth(gl_FragCoord.z, cameraFar, cameraNear) / cameraFar;		
		fragColor = vec4(vec3(depth), 1);
	}
	else
	{
		vec4 texColor = texture(material.diffuse, TexCoords);
		if(texColor.a < 0.1)
			discard;
		fragColor = texColor;
	}
}
#endif
