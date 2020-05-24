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
 
#include "common.glh"
#include <uniformBuffers.glh>
#include <utility.glh>

#if defined(VS_BUILD)

Layout(0) attribute vec3 position;
Layout(1) attribute vec2 texCoord;
Layout(2) attribute vec3 normal;
Layout(3) attribute vec3 tangent;
Layout(4) attribute vec3 biTangent;
Layout(5) attribute mat4 model;
Layout(9) attribute mat4 inverseTransposeModel;

out VS_OUT
{
	vec3 Normal;
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

#include "lightingData.glh"


void main()
{
	vec3 viewPos = vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z);

	vs_out.FragPos = vec3(model * vec4(position,1.0));
    vs_out.TexCoords = texCoord;
	vs_out.Normal = mat3(inverseTransposeModel) * normal;
	
	mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * tangent);
    vec3 N = normalize(normalMatrix * normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));    
    vs_out.TangentLightPos = TBN * pointLights[0].position;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;
	
    gl_Position = projection * view * model * vec4(position, 1.0);
	
}

#elif defined(GEO_BUILD)

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 Normal;
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} gs_in[];

out vec2 TexCoords; 

uniform float time;


void main() {    
    
}

#elif defined(FS_BUILD)

struct MaterialSampler2D
{
	sampler2D texture;
	int isActive;
};

struct Material
{
MaterialSampler2D diffuse;
MaterialSampler2D normalMap;
int specularExponent;
vec3 objectColor;
int surfaceType;
vec2 tiling;
};

uniform Material material;


in VS_OUT
{
	vec3 Normal;
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

#include "lightingData.glh"
#include "lightingCalc.glh"



out vec4 fragColor;

void main()
{
	
 
	if(visualizeDepth)
	{
		float depth = LinearizeDepth(gl_FragCoord.z, cameraFar, cameraNear) / cameraFar;		
		fragColor = vec4(vec3(depth), 1);
	}
	else
	{
		// General
		vec3 viewPos = vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z);
		vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
		vec2 tiledTexCoord = vec2(fs_in.TexCoords.x * material.tiling.x, fs_in.TexCoords.y * material.tiling.y);
		
		// Textures
		vec4 diffuseTextureColor =  material.diffuse.isActive != 0 ? texture(material.diffuse.texture, tiledTexCoord) : vec4(1,1,1,1);
		vec3 normal = texture(material.normalMap.texture, tiledTexCoord).rgb;
		
		if(material.normalMap.isActive != 0)
			normal = normalize(normal * 2.0 - 1.0);
	
		// Colors
		vec3 finalColor = diffuseTextureColor.rgb * material.objectColor;
		vec3 lighting = vec3(0.0);
		
		// Lighting
		for(int i = 0; i < pointLightCount; i++)
			lighting += CalculatePointLight(pointLights[i], normal, fs_in.FragPos, viewDir, (fs_in.TangentLightPos - fs_in.TangentFragPos));    
		//for(int i = 0; i < spotLightCount; i++)
			//lighting += CalculateSpotLight(spotLights[i], normal, fs_in.FragPos, viewDir);   
			
		// Post pp
		finalColor *= lighting;
		finalColor =  pow(finalColor, vec3(1.0/2.2));
		
		// Set fragment
		float alpha = material.surfaceType == 0 ? 1.0 : diffuseTextureColor.a;
		fragColor = vec4(finalColor, alpha);
	
	
	}
}
#endif
