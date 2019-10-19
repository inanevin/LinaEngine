
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
 
#include <common.glh>

#if defined(VS_BUILD)
layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;

out vec3 rawPosition;
out vec4 vertexPos;

void main()
{
    vec4 pos = projection * view * vec4(position, 1.0);
    gl_Position = pos.xyww;
	vertexPos = projection * vec4(position, 1.0);
	rawPosition = position;
}
  
  
#elif defined(FS_BUILD)

in vec3 rawPosition;
in vec4 vertexPos;
out vec4 fragColor;


uniform vec3 startColor;
uniform vec3 endColor;

float remap( float value, float low1, float high1, float low2, float high2)
{
    return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
} 

void main()
{    
	float u = rawPosition.y;
	u = remap(u, -1.0f, 1.0f, 0.0f, 1.0f);
    gl_FragColor = mix( vec4(startColor, 1.0), vec4(endColor, 1.0), u );
}
#endif

