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

layout (location = 0) out vec4 gPosition;		// rgb = position
layout (location = 1) out vec4 gNormal;			// rgb = normal
layout (location = 2) out vec4 gAlbedo;					
layout (location = 3) out vec4 gEmission;		    				// rgb = emission, a = workflow
layout (location = 4) out vec4 gMetallicRoughnessAOWorkflow;		// r = metallic, g = roughness, b = ao, a = workflow
layout (location = 5) out vec4 gReflection;		

in vec3 WorldPos;

struct Material
{
  MaterialSamplerCube environmentMap;
};
uniform Material material;

void main()
{
  vec3 envColor = material.environmentMap.isActive ? texture(material.environmentMap.texture, WorldPos).rgb : vec3(1.0);

  // HDR tonemap and gamma correct
  //envColor = envColor / (envColor + vec3(1.0));
  //envColor = pow(envColor, vec3(1.0/2.2));

  gPosition = vec4(WorldPos, 1.0f);
  gNormal = vec4(0.0f);
  gAlbedo = vec4(envColor, 1.0f);
  gEmission = vec4(0.0f);
  gMetallicRoughnessAOWorkflow = vec4(0.0f, 0.0f, 0.0f, 2.0f); // unlit
	
}

#endif
