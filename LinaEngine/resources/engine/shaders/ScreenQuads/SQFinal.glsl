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
  MaterialSampler2D bloomMap;
  MaterialSampler2D outlineMap;
  vec3 inverseScreenMapSize;
  float exposure;
  bool bloomEnabled;
  bool fxaaEnabled;
  float fxaaSpanMax;
  float fxaaReduceMin;
  float fxaaReduceMul;
};
uniform Material material;

void main()
{

  if(material.screenMap.isActive)
  {
    const float gamma = 2.2;
    vec3 hdrColor = texture(material.screenMap.texture, TexCoords).rgb;

    if(material.fxaaEnabled)
    {
      vec3 fxaaColor = vec3(0.0);

      vec2 tcOffset = material.inverseScreenMapSize.xy;

      // Get lumas
      vec3 luma = vec3(0.299, 0.587, 0.114);
      float lumaTL = dot(luma, texture(material.screenMap.texture, TexCoords.xy + vec2(-1.0, -1.0) * tcOffset).xyz);
      float lumaTR = dot(luma, texture(material.screenMap.texture, TexCoords.xy + vec2(1.0, -1.0) * tcOffset).xyz);
      float lumaBL = dot(luma, texture(material.screenMap.texture, TexCoords.xy + vec2(-1.0, 1.0) * tcOffset).xyz);
      float lumaBR = dot(luma, texture(material.screenMap.texture, TexCoords.xy + vec2(1.0, 1.0) * tcOffset).xyz);
      float lumaM = dot(luma, texture(material.screenMap.texture, TexCoords.xy).xyz);


      // If pixel is edge, we'll have some magnitude in blurDirection vector.
      vec2 blurDirection;
      blurDirection.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));  // Horizontal sum
      blurDirection.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));  // Vertical sum

      // Scale direction vector acc to smallest component.
      float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (material.fxaaReduceMul * 0.25), material.fxaaReduceMin);
      float inverseDirAdj = 1.0 / min(abs(blurDirection.x), abs(blurDirection.y) + dirReduce);
      blurDirection = min(vec2(material.fxaaSpanMax), max(vec2(-material.fxaaSpanMax), blurDirection * inverseDirAdj)) * tcOffset;

      vec3 result1 = (1.0 / 2.0) * (
        texture(material.screenMap.texture, TexCoords.xy + (blurDirection * vec2(1.0/3.0 - 0.5))).xyz +
        texture(material.screenMap.texture, TexCoords.xy + (blurDirection * vec2(2.0/3.0 - 0.5))).xyz);

      vec3 result2 = result1  * (1.0/2.0) + (1.0 / 4.0) * (
        texture(material.screenMap.texture, TexCoords.xy + (blurDirection * vec2(0.0/3.0 - 0.5))).xyz +
        texture(material.screenMap.texture, TexCoords.xy + (blurDirection * vec2(3.0/3.0 - 0.5))).xyz);

      // Test if we've sampled too far.
      float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
      float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
      float lumaResult2 = dot(luma, result2);
      hdrColor = (lumaResult2 < lumaMin || lumaResult2 > lumaMax) ? result1 : result2;
    }

    // Add bloom.
    if(material.bloomEnabled && material.bloomMap.isActive)
      hdrColor += texture(material.bloomMap.texture, TexCoords).rgb;

    // Add outline
    if(material.outlineMap.isActive)
      hdrColor += texture(material.outlineMap.texture, TexCoords).rgb;


    // tone mapping
    //vec3 result = vec3(1.0) - exp(-hdrColor * material.exposure);
    // also gamma correct while we're at it
    //result = pow(result, vec3(1.0 / gamma));
    fragColor = vec4(hdrColor, 1.0);
  }
  else
    fragColor = vec4(1.0,0.0,0.0,1.0);


}
#endif
