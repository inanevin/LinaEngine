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
Layout(4) attribute mat4 model;

void main()
{
    gl_Position = model * vec4(position, 1.0);
}

#elif defined(GS_BUILD)

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle vertex
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}  

#elif defined(FS_BUILD)
#define BIAS 0.01

in vec4 FragPos;

uniform vec3 lightPosition;
uniform float farPlane;

void main()
{
float fp = 12.5f;
vec3 lp = vec3(0,4,0);
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - lp);
    
    // map to [0;1] range by dividing by farPlane
    lightDistance = lightDistance / fp;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}  
#endif
