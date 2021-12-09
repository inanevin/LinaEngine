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
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 biTangent;
layout (location = 7) in mat4 model;
layout (location = 11) in mat4 inverseTransposeModel;

void main()
{
    gl_Position = model * vec4(position, 1.0);
}

#elif defined(GS_BUILD)
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;
out vec4 FragPos; // FragPos from GS (output per emitvertex)
uniform mat4 uf_shadowMatrices[6];

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = uf_shadowMatrices[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
} 

#elif defined(FS_BUILD)

in vec4 FragPos;

uniform vec3 uf_lightPos;
uniform float uf_lightFarPlane;

void main()
{
    float lightDistance = length(FragPos.xyz - uf_lightPos);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / uf_lightFarPlane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}

#endif
