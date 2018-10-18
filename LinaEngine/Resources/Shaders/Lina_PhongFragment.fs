#version 330 core


//GLSL doesn't have any built in color passing variable so we declare for ourselves as build in vec4 type.
in vec2 texCoord0;
in vec3 normal0;
out vec4 fragColor;

struct BaseLight
{
	vec3 color;
	float intensity;
};

struct DirectionalLight
{
	BaseLight base;
	vec3 direction;
};


uniform vec3 baseColor;
uniform vec3 ambientLight;
uniform sampler2D sampler;
uniform DirectionalLight directionalLight;


vec4 CalculateLight(BaseLight base, vec3 dir, vec3 normal)
{
	float diffuseFac = dot(-dir, normal);
	
	vec4 diffuseColor = vec4(0,0,0,0);
	
	// If our light is affecting our surface.
	if(diffuseFac > 0)
	{
		diffuseColor = vec4(base.color, 1.0) * base.intensity * diffuseFac;
	}
	
	return diffuseColor;
}

vec4 CalculateDirectionalLight(DirectionalLight dirLight, vec3 normal)
{
	return CalculateLight(dirLight.base, dirLight.direction, normal);
}

void main()
{
	vec4 totalLight = vec4(ambientLight,1);
	vec4 color = vec4(baseColor,1);
	vec4 textureColor = texture(sampler, texCoord0.xy);
	
	if(textureColor != vec4(0,0,0,0))
		color *= textureColor;
		
	vec3 normal = normalize(normal0);
	totalLight += CalculateDirectionalLight(directionalLight, normal);
	
	fragColor = color * totalLight;
}