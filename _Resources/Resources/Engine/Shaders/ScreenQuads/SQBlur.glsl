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
