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

void main()
{
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    TexCoords = texCoord;
}

#elif defined(FS_BUILD)

in vec2 TexCoords;
out vec4 fragColor;

struct MaterialSampler2D
{
	sampler2D texture;
	int isActive;
};

uniform MaterialSampler2D screenTexture;


void main()
{
	float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
	vec2 tex_offset = 1.0 / textureSize(screenTexture.texture, 0); // gets size of single texel
    vec3 result = screenTexture.isActive != 0 ? texture(screenTexture.texture, TexCoords).rgb : vec3(1,0,1); // current fragment's contribution
    fragColor = vec4(result * vec3(1,1,0), 1.0);
}
#endif
