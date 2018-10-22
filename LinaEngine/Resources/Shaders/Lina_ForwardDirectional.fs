#version 120

varying vec2 texCoord0;
varying vec3 normal0;
varying vec3 worldPos0;

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

uniform float specularIntensity;
uniform float specularExponent;
uniform vec3 camPos;
uniform sampler2D diffuse;

uniform DirectionalLight directionalLight;

vec4 CalculateLight(BaseLight base, vec3 dir, vec3 normal)
{
	float diffuseFac = dot(normal, -dir);
	
	vec4 diffuseColor = vec4(0,0,0,0);
	vec4 specColor = vec4(0,0,0,0);
	
	// If our light is affecting our surface.
	if(diffuseFac > 0)
	{
		diffuseColor = vec4(base.color, 1.0) * base.intensity * diffuseFac;
		
		vec3 dirToCam = normalize(camPos - worldPos0); // dir to camera
		vec3 reflectDir = normalize(reflect(dir, normal));	// reflected dir of the incoming light dir
		float specFactor = dot(dirToCam, reflectDir); // diverge as the dot expands
		specFactor = pow(specFactor, specularExponent);	// Apply exponent
		
		if(specFactor > 0)
		{
			specColor = vec4(base.color, 1.0) * specularIntensity * specFactor;
		}
	}
	
	return diffuseColor + specColor;
}

vec4 CalculateDirectionalLight(DirectionalLight dirLight, vec3 normal)
{
	return CalculateLight(dirLight.base, -dirLight.direction, normal);
}

void main()
{
	gl_FragColor = 
	texture2D(diffuse, texCoord0.xy) *
	CalculateDirectionalLight(directionalLight, normalize(normal0));
}
