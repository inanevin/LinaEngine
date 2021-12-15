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
layout (location = 0) in vec3 position;
out vec3 WorldPos;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    WorldPos = position;
    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}


#elif defined(FS_BUILD)
#include <../MaterialSamplers.glh>
out vec4 fragColor;
in vec3 WorldPos;
struct Material
{
  MaterialSamplerCube environmentMap;
};
uniform Material material;

const float PI = 3.14159265359;
void main()
{
  // The world vector acts as the normal of a tangent surface
  // from the origin, aligned to WorldPos. Given this normal, calculate all
  // incoming radiance of the environment. The result of this radiance
  // is the radiance of light coming from -Normal direction, which is what
  // we use in the PBR shader to sample irradiance.
  vec3 N = normalize(WorldPos);

  vec3 irradiance = vec3(0.0);

  // tangent space calculation from origin point
  vec3 up    = vec3(0.0, 1.0, 0.0);
  vec3 right = cross(up, N);
  up            = cross(N, right);

  float sampleDelta = 0.025;
  float nrSamples = 0.0;
  for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
  {
      for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
      {
          // spherical to cartesian (in tangent space)
          vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
          // tangent space to world
          vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
          vec3 textureColor = material.environmentMap.isActive  ? texture(material.environmentMap.texture, sampleVec).rgb : vec3(0.0);

          irradiance += textureColor * cos(theta) * sin(theta);
          nrSamples++;
      }
  }

  irradiance = PI * irradiance * (1.0 / float(nrSamples));
  fragColor = vec4(irradiance, 1.0);
}
#endif
