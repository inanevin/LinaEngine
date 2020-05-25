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
uniform MaterialSampler2D bloomTexture;
uniform MaterialSampler2D outlineTexture;
uniform bool bloom;
uniform float exposure;
const float offset = 1.0 / 300.0;

void main()
{

	const float gamma = 2.2;
  vec3 hdrColor = screenTexture.isActive != 0 ? texture(screenTexture.texture, TexCoords).rgb : vec3(0.0);
  vec3 bloomColor = bloomTexture.isActive != 0 ? texture(bloomTexture.texture, TexCoords).rgb : vec3(0.0);
	vec3 outlineColor = outlineTexture.isActive != 0 ? texture(outlineTexture.texture, TexCoords).rgb : vec3(0.0);

	// Add bloom.
	if(bloom)
        hdrColor += bloomColor;

	  // Add outline
	  hdrColor += outlineColor;

	   // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it
    result = pow(result, vec3(1.0 / gamma));
    fragColor = vec4(result, 1.0);
}
#endif
