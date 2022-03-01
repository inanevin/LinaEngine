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
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 biTangent;
layout (location = 7) in mat4 model;

out vec2 TexCoords;
out vec3 FragPos;

void main()
{
  gl_Position = LINA_VP * model * vec4(position, 1.0);
  FragPos = vec3(model * vec4(position,1.0));
  TexCoords = texCoords;
}

#elif defined(FS_BUILD)
#include <../UniformBuffers.glh>
#include <../Utility.glh>
#include <../MaterialSamplers.glh>

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;
in vec3 FragPos;
in vec2 TexCoords;

struct Material
{
  MaterialSampler2D diffuse;
  vec3 objectColor;
  int surfaceType;
  
};

uniform Material material;


void main()
{
	float alpha = material.surfaceType == 0 ? 1.0 : (material.diffuse.isActive ? texture(material.diffuse.texture, TexCoords).a : 1.0);

	vec4 color = (material.diffuse.isActive ? texture(material.diffuse.texture ,TexCoords) : vec4(1.0)) * vec4(material.objectColor, 1.0);
	fragColor = vec4(color.rgb, alpha);
		
	float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0)
		brightColor = vec4(fragColor.rgb, 1.0);
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
#endif
