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

uniform float equatorRange;
uniform samplerCube skybox;

float remap( float minval, float maxval, float curval )
{
    return ( curval - minval ) / ( maxval - minval );
} 

const vec4 GREEN = vec4( 0.0, 1.0, 0.0, 1.0 );
const vec4 WHITE = vec4( 1.0, 1.0, 1.0, 1.0 );
const vec4 RED   = vec4( 1.0, 0.0, 0.0, 1.0 );

void main()
{    
	float u = rawPosition.y;
	u = clamp( u, 0.0, 1.0 );
	
	if( u < 0.5 )
    gl_FragColor = mix( GREEN, WHITE, remap( 0.0, 0.5, u ) );
else
    gl_FragColor = mix( WHITE, RED, remap( 0.5, 1.0, u ) );
	
}
#endif
