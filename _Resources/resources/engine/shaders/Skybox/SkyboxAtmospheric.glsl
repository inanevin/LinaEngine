
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
out vec3 fsun;
out vec3 pos;
uniform float uf_time = 0.0;

const vec2 data[4] = vec2[](
    vec2(-1.0,  1.0), vec2(-1.0, -1.0),
    vec2( 1.0,  1.0), vec2( 1.0, -1.0));
	
void main()
{
	mat4 rotView = mat4(mat3(LINA_VIEW));
    vec4 clipPos = LINA_PROJ * rotView * vec4(position, 1.0);
    // gl_Position = clipPos.xyww;
	gl_Position = vec4(data[gl_VertexID], 0.0, 1.0);
	pos = transpose(mat3(LINA_VIEW)) * (inverse(LINA_PROJ) * gl_Position).xyz;
    fsun = vec3(0.0, sin(uf_time * 0.01), cos(uf_time * 0.01));
}

#elif defined(FS_BUILD)
in vec3 pos;
in vec3 fsun;
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

struct Material
{
	float time;
	float cirrus;
	float cumulus;
};

uniform Material material;

  
float hash(float n)
{
	return fract(sin(n) * 43758.5453123);
}

float noise(vec3 x)
{
	vec3 f = fract(x);
	float n = dot(floor(x), vec3(1.0, 157.0, 113.0));
	return mix(mix(mix(hash(n +   0.0), hash(n +   1.0), f.x),
			   mix(hash(n + 157.0), hash(n + 158.0), f.x), f.y),
		   mix(mix(hash(n + 113.0), hash(n + 114.0), f.x),
			   mix(hash(n + 270.0), hash(n + 271.0), f.x), f.y), f.z);
}

const mat3 m = mat3(0.0, 1.60,  1.20, -1.6, 0.72, -0.96, -1.2, -0.96, 1.28);
float fbm(vec3 p)
{
	float f = 0.0;
	f += noise(p) / 2; p = m * p * 1.1;
	f += noise(p) / 4; p = m * p * 1.2;
	f += noise(p) / 6; p = m * p * 1.3;
	f += noise(p) / 12; p = m * p * 1.4;
	f += noise(p) / 24;
	return f;
}

const float Br = 0.0025;
const float Bm = 0.0003;
const float g =  0.9800;
const vec3 nitrogen = vec3(0.650, 0.570, 0.475);
const vec3 Kr = Br / pow(nitrogen, vec3(4.0));
const vec3 Km = Bm / pow(nitrogen, vec3(0.84));

void main()
{
	if (pos.y < 0)
	discard;

	// Atmosphere Scattering
	float mu = dot(normalize(pos), normalize(fsun));
	vec3 extinction = mix(exp(-exp(-((pos.y + fsun.y * 4.0) * (exp(-pos.y * 16.0) + 0.1) / 80.0) / Br) * (exp(-pos.y * 16.0) + 0.1) * Kr / Br) * exp(-pos.y * exp(-pos.y * 8.0 ) * 4.0) * exp(-pos.y * 2.0) * 4.0, vec3(1.0 - exp(fsun.y)) * 0.2, -fsun.y * 0.2 + 0.5);
	fragColor.rgb = 3.0 / (8.0 * 3.14) * (1.0 + mu * mu) * (Kr + Km * (1.0 - g * g) / (2.0 + g * g) / pow(1.0 + g * g - 2.0 * g * mu, 1.5)) / (Br + Bm) * extinction;

	// Cirrus Clouds
	float density = smoothstep(1.0 - material.cirrus, 1.0, fbm(pos.xyz / pos.y * 2.0 + material.time * 0.05)) * 0.3;
	fragColor.rgb = mix(fragColor.rgb, extinction * 4.0, density * max(pos.y, 0.0));

	// Cumulus Clouds
	for (int i = 0; i < 3; i++)
	{
	float density = smoothstep(1.0 - material.cumulus, 1.0, fbm((0.7 + float(i) * 0.01) * pos.xyz / pos.y + material.time * 0.3));
	fragColor.rgb = mix(fragColor.rgb, extinction * density * 5.0, min(density, 1.0) * max(pos.y, 0.0));
	}

	// Dithering Noise
	fragColor.rgb += noise(pos * 1000) * 0.01;
	
	float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        brightColor = vec4(fragColor.rgb, 1.0);
    else
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
		
	// HDR tonemap and gamma correct
	//fragColor.xyz = fragColor.xyz / (fragColor.xyz + vec3(1.0));
	//fragColor.xyz = pow(fragColor.xyz, vec3(1.0/2.2));
	
}
  
#endif
