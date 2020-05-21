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

uniform sampler2D screenTexture;

void main()
{
	//vec3 color = texture(screenTexture, TexCoords).xyz;
	//color = pow(color, vec3(1.0/2.2));
	//fragColor = vec4(color, texture(screenTexture, TexCoords).a);
	//fragColor = texture(screenTexture, TexCoords);
	float depthVal = texture(screenTexture, TexCoords).r;
	//fragColor = vec4(vec3(LinearizeDepth(depthVal, 1000, 0.1) / 1000), 1.0); 
	
	fragColor = vec4(vec3(depthVal), 1.0);
}
#endif
