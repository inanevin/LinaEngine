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
out vec2 TexCoords;

void main()
{
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    TexCoords = texCoords;
}

#elif defined(FS_BUILD)
#include <../MaterialSamplers.glh>
out vec4 fragColor;
in vec2 TexCoords;

struct Material
{
	MaterialSampler2D screenMap;
	bool horizontal;
};
uniform Material material;

void main()
{
  vec3 result = vec3(0,1,0);

  if(material.screenMap.isActive)
  {
    float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
    vec2 tex_offset = 1.0 / textureSize(material.screenMap.texture, 0); // gets size of single texel
    result = texture(material.screenMap.texture, TexCoords).rgb * weight[0]; // current fragment's contribution
    if(material.horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(material.screenMap.texture, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(material.screenMap.texture, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(material.screenMap.texture, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(material.screenMap.texture, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
  }

  fragColor = vec4(result, 1.0);
}
#endif
