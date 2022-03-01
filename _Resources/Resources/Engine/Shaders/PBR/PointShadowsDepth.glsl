/*
*	This file is a part of: Lina Engine
*	https://github.com/inanevin/LinaEngine
*	
*	Author: Inan Evin
*	http://www.inanevin.com
*	
*	Copyright (c) [2018-] [Inan Evin]
*	
*	Permission is hereby granted, free of charge, to any person obtaining a copy
*	of this software and associated documentation files (the "Software"), to deal
*	in the Software without restriction, including without limitation the rights
*	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*	copies of the Software, and to permit persons to whom the Software is
*	furnished to do so, subject to the following conditions:
*	
*	The above copyright notice and this permission notice shall be included in all
*	copies or substantial portions of the Software.
*	
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*	SOFTWARE.
*/

#if defined(VS_BUILD)
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 biTangent;
layout (location = 7) in mat4 model;

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
