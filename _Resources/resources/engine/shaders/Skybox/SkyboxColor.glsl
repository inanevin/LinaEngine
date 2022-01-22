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
#include <SkyboxCommon.glh>

void main()
{
  gl_Position = vec4(data[gl_VertexID], 0.0, 1.0);
}

#elif defined(FS_BUILD)

layout (location = 0) out vec4 gPositionMetallic;		// rgb = position, a = metallic
layout (location = 1) out vec4 gNormalRoughness;		// rgb = normal, a = roughness
layout (location = 2) out vec4 gAlbedoAO;				// rgb = albedo, a = AO
layout (location = 3) out vec4 gEmissionWorkflow;		// rgb = emission, a = workflow

struct Material
{
  vec3 color;
};
uniform Material material;

void main()
{   
  gPositionMetallic = vec4(vec3(0), 0.0f);
  gNormalRoughness = vec4(0.0f);
  gAlbedoAO = vec4(vec3(material.color.x, material.color.y, material.color.z), 0.0f);
  gEmissionWorkflow = vec4(vec3(0.0f), 2.0f);	// Unlit
}
#endif
