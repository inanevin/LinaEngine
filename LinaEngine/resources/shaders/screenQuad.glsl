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

uniform sampler2DMS screenTexture;

void main()
{
//texelFetch requires a vec of ints for indexing (since we're indexing pixel locations)
					//texture coords is range [0, 1], we need range [0, viewport_dim].
					//texture coords are essentially a percentage, so we can multiply text coords by total size 
					ivec2 vpCoords = ivec2(1440, 900);
					vpCoords.x = int(vpCoords.x * TexCoords.x); 
					vpCoords.y = int(vpCoords.y * TexCoords.y);

					//do a simple average since this is just a demo
					vec4 sample1 = texelFetch(screenTexture, vpCoords, 0);
					vec4 sample2 = texelFetch(screenTexture, vpCoords, 1);
					vec4 sample3 = texelFetch(screenTexture, vpCoords, 2);
					vec4 sample4 = texelFetch(screenTexture, vpCoords, 3);
					fragColor = (sample1 + sample2 + sample3 + sample4) / 4.0f;
	//fragColor = texture(screenTexture, TexCoords);
}
#endif
