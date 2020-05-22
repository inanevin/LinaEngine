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
Layout(4) attribute mat4 model;
Layout(8) attribute mat4 inverseTransposeModel;

out VS_OUT
{
	vec3 Normal;
	vec3 FragPos;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} vs_out;

void main()
{
    //gl_Position = projection * view * model * vec4(position, 1.0);
	//vs_out.FragPos = vec3(model * vec4(position,1.0));
	//vs_out.Normal = mat3(inverseTransposeModel) * normal;
    //vs_out.TexCoords = texCoord;
	
	vs_out.FragPos = vec3(model * vec4(position, 1.0));
    vs_out.Normal = mat3(inverseTransposeModel) * normal;
    vs_out.TexCoords = texCoord;
    vs_out.FragPosLightSpace = lightSpace * vec4(vs_out.FragPos, 1.0);
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}

#elif defined(GEO_BUILD)

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 Normal;
	vec3 FragPos;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} gs_in[];

out vec2 TexCoords; 

uniform float time;

vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0;
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude; 
    return position + vec4(direction, 0.0);
}

vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

void main() {    
    vec3 normal = GetNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
    TexCoords = gs_in[0].TexCoords;
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, normal);
    TexCoords = gs_in[1].TexCoords;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, normal);
    TexCoords = gs_in[2].TexCoords;
    EmitVertex();
    EndPrimitive();
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
MaterialSampler2D shadowMap;
vec3 objectColor;
float specularIntensity;
int specularExponent;
int surfaceType;
vec2 tiling;
};

uniform Material material;


in VS_OUT
{
	vec3 Normal;
	vec3 FragPos;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} fs_in;

#include "lighting.glh"



out vec4 fragColor;

void main()
{
	
	vec3 viewPos = vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z);
	//vec3 norm = normalize(fs_in.Normal);
	//vec3 viewDir = normalize(viewPos - fs_in.FragPos);

 
	if(visualizeDepth)
	{
		float depth = LinearizeDepth(gl_FragCoord.z, cameraFar, cameraNear) / cameraFar;		
		fragColor = vec4(vec3(depth), 1);
	}
	else
	{

	vec4 diffuseTextureColor =  material.diffuse.isActive != 0 ? texture(material.diffuse.texture, vec2(fs_in.TexCoords.x * material.tiling.x, fs_in.TexCoords.y * material.tiling.y)) : vec4(1,1,1,1);
	vec3 lightPos = directionalLight.position;
	vec3 color = diffuseTextureColor.rgb * material.objectColor;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = directionalLight.color;
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), material.specularExponent);
    vec3 specular = spec * lightColor * material.specularIntensity;    
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, vec3(-2.0, 4.0, -1.0));                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
	lighting = pow(lighting, vec3(1.0/2.2));	
	float alpha = material.surfaceType == 0 ? 1.0 : diffuseTextureColor.a;
	
    fragColor = vec4(lighting, 1.0);
	
	
	}
}
#endif
