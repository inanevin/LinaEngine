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
  MaterialSampler2D bloomMap;
  vec3 inverseScreenMapSize;
  bool bloomEnabled;
  bool fxaaEnabled;
  bool vignetteEnabled;
  float fxaaSpanMax;
  float fxaaReduceMin;
  float fxaaReduceMul;
  float exposure;
  float gamma;
  float vignetteAmount;
  float vignettePow;
};
uniform Material material;

float move(float x)
	{
		return abs(1.0 - mod(abs(x), 2.0)) * 5 - (5);
	}
	
void main()
{
  if(material.screenMap.isActive)
  {

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

	if(material.vignetteEnabled)
	{
		vec2 uv = TexCoords;
		uv *= 1.0 - uv.yx;
		float vig = uv.x*uv.y * material.vignetteAmount;
		vig = pow(vig, material.vignettePow); // change pow for modifying the extend of the  vignette
		hdrColor *= vec3(vig, vig, vig);
	}
	
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * material.exposure);
    // also gamma correct while we're at it
    result = pow(result, vec3(1.0 / material.gamma));
	//vec3 result = pow(1.0 - exp(-material.exposure * hdrColor.rgb), vec3(material.exposure));
    fragColor = vec4(result, 1.0);
  }
  else
    fragColor = vec4(1.0,0.0,0.0,1.0);


}
#endif
