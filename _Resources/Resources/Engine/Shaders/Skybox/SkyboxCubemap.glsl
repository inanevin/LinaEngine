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
#include <../UniformBuffers.glh>
#include <SkyboxCommon.glh>
out vec3 WorldPos;

	
void main()
{
	mat4 inverseProjection = inverse(LINA_PROJ);
    mat3 inverseModelview = transpose(mat3(LINA_VIEW));
    vec3 unprojected = (inverseProjection * vec4(data[gl_VertexID], 0.0, 1.0)).xyz;
    vec3 eyeDirection = inverseModelview * unprojected;
    gl_Position = vec4(data[gl_VertexID], 0.0, 1.0);
    WorldPos = eyeDirection;
}

#elif defined(FS_BUILD)
#include <../MaterialSamplers.glh>
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;
in vec3 WorldPos;
struct Material
{
  MaterialSamplerCube environmentMap;
};
uniform Material material;

void main()
{
   fragColor = material.environmentMap.isActive ? texture(material.environmentMap.texture, WorldPos) : vec4(1.0);
   
   float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0)
		brightColor = vec4(fragColor.rgb, 1.0);
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
#endif
