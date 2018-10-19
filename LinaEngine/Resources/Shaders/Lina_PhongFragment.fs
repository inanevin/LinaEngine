#version 330 core


//GLSL doesn't have any built in color passing variable so we declare for ourselves as build in vec4 type.
in vec2 texCoord0;
in vec3 normal0;
in vec3 worldPos0;
out vec4 fragColor;

const int POINT_LIGHT_PIXEL_COUNT = 4;
const int SPOT_LIGHT_PIXEL_COUNT = 4;

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

// How quickly point light fades out, weakens
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
uniform vec3 baseColor;
uniform vec3 ambientLight;
uniform sampler2D sampler;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[POINT_LIGHT_PIXEL_COUNT];
uniform SpotLight spotLights[SPOT_LIGHT_PIXEL_COUNT];




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
	vec4 totalLight = vec4(ambientLight,1);
	vec4 color = vec4(baseColor,1);
	vec4 textureColor = texture(sampler, texCoord0.xy);
	
	if(textureColor != vec4(0,0,0,0))
		color *= textureColor;
		
	vec3 normal = normalize(normal0);
	
	totalLight += CalculateDirectionalLight(directionalLight, normal);
	
	for(int i = 0; i < POINT_LIGHT_PIXEL_COUNT; i++)
	{
		if(pointLights[i].base.intensity > 0)
			totalLight += CalculatePointLight(pointLights[i], normal);
	}
	
	
	for(int i = 0; i < SPOT_LIGHT_PIXEL_COUNT; i++)
	{
		if(spotLights[i].pointLight.base.intensity > 0)
			totalLight += CalculateSpotLight(spotLights[i], normal);
	}
	
	fragColor = color * totalLight;
}