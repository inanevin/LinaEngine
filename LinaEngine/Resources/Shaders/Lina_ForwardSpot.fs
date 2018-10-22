#version 120

varying vec2 texCoord0;
varying vec3 normal0;
varying vec3 worldPos0;

struct BaseLight
{
	vec3 color;
	float intensity;
};

struct Attenuation
{
	float constant;
	float linear;
	float exponent;
};

struct PointLight
{
	BaseLight base;
	Attenuation attenuation;
	vec3 position;
	float range;
};

struct SpotLight
{
	vec3 direction;
	float cutoff;
	PointLight pointLight;
};

uniform float specularIntensity;
uniform float specularExponent;
uniform vec3 camPos;
uniform sampler2D diffuse;
uniform SpotLight spotLight;

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

vec4 CalculatePointLight(PointLight pLight, vec3 normal)
{
	vec3 lDir = worldPos0 - pLight.position;	// pixel pos - light pos
	float distToLight = length(lDir);
	
	if(distToLight > pLight.range)
		return vec4(0,0,0,0);
	
	lDir = normalize(lDir);
	
	vec4 color = CalculateLight(pLight.base, lDir, normal);
	
	float atten = pLight.attenuation.constant + pLight.attenuation.linear * distToLight + pLight.attenuation.exponent * distToLight * distToLight + 0.00001f;	// prevent division by zero
	
	return color/atten;
}

vec4 CalculateSpotLight(SpotLight sLight, vec3 normal)
{
	vec3 lightDir = normalize(worldPos0 - sLight.pointLight.position);
	float spotMultiplier = dot(lightDir, sLight.direction);
	
	vec4 color = vec4(0,0,0,0);
	
	if(spotMultiplier > sLight.cutoff)
	{
		color = CalculatePointLight(sLight.pointLight, normal) * (1.0 - (1.0 - spotMultiplier) / (1.0- sLight.cutoff));	// Last part is interpolation.
	}
	
	return color;
}

void main()
{
	gl_FragColor = 
	texture2D(diffuse, texCoord0.xy) *
	CalculateSpotLight(spotLight, normalize(normal0));
}
