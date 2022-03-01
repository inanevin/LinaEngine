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


layout (location = 0) out vec4 gPosition;		// rgb = position
layout (location = 1) out vec4 gNormal;			// rgb = normal
layout (location = 2) out vec4 gAlbedo;					
layout (location = 3) out vec4 gEmission;		    				// rgb = emission, a = workflow
layout (location = 4) out vec4 gMetallicRoughnessAOWorkflow;		// r = metallic, g = roughness, b = ao, a = workflow

in vec3 FragPos;
in vec2 TexCoords;

struct Material
{
  MaterialSampler2D diffuse;
  vec4 objectColor;
  int surfaceType;
  vec2 tiling;
};

uniform Material material;


void main()
{
    vec2 tiled = vec2(TexCoords.x * material.tiling.x, TexCoords.y * material.tiling.y);
	vec4 color = material.diffuse.isActive ? texture(material.diffuse.texture ,tiled) * material.objectColor : material.objectColor;
	float alpha = material.surfaceType == 0 ? 1.0 : color.a;

	gPosition = vec4(FragPos, 1.0f);
	gNormal = vec4(0.0f);
	gAlbedo = vec4(color.rgb, alpha);
	gEmission = vec4(0.0f);
	gMetallicRoughnessAOWorkflow = vec4(0.0f, 0.0f, 0.0f, 2.0f); // unlit
}
#endif
