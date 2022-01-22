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

layout (location = 0) out vec4 gPositionMetallic;		// rgb = position, a = metallic
layout (location = 1) out vec4 gNormalRoughness;		// rgb = normal, a = roughness
layout (location = 2) out vec4 gAlbedoAO;				// rgb = albedo, a = AO
layout (location = 3) out vec4 gEmissionWorkflow;		// rgb = emission, a = workflow
in vec3 FragPos;
in vec2 TexCoords;

struct Material
{
  MaterialSampler2D diffuse;
  vec3 objectColor;
  int surfaceType;
  vec2 tiling;
};

uniform Material material;


void main()
{
    vec2 tiled = vec2(TexCoords.x * material.tiling.x, TexCoords.y * material.tiling.y);
	float alpha = material.surfaceType == 0 ? 1.0 : (material.diffuse.isActive ? texture(material.diffuse.texture, tiled).a : 1.0);

	vec4 color = (material.diffuse.isActive ? texture(material.diffuse.texture ,tiled) : vec4(1.0)) * vec4(material.objectColor, 1.0);
	color.a = alpha;
	
	gPositionMetallic = vec4(FragPos, 0.0f);
	gNormalRoughness = vec4(0.0f);
	gAlbedoAO = vec4(color.rgb, 0.0f);
	gEmissionWorkflow = vec4(vec3(0.0f), 2.0f);	// Unlit
}
#endif
