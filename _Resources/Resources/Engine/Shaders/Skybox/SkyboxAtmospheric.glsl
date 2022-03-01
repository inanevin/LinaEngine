/*
*	This file is a part of: Lina Engine
*	https://github.com/inanevin/LinaEngine
*	
*	Author: Inan Evin
*	http://www.inanevin.com
*	
*	Copyright (c) [2018-] [Inan Evin]
*	
*	The atmospheric skybox algorithm by:
* 	(c) 2019 FMS_Cat, MIT License
*	https://www.shadertoy.com/view/3djSzz
* 	https://opensource.org/licenses/MIT
* 	Based on "Accurate Atmospheric Scattering" from GPU Gems2
* 	https://developer.nvidia.com/gpugems/GPUGems2/gpugems2_chapter16.html

*	Permission is hereby granted, free of charge, to any person obtaining a copy
*	of this software and associated documentation files (the "Software"), to deal
*	in the Software without restriction, including without limitation the rights
*	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*	copies of the Software, and to permit persons to whom the Software is
*	furnished to do so, subject to the following conditions:
*	
*	The above copyright notice and this permission notice shall be included in all
*	copies or substantial portions of the Software.
*	
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*	SOFTWARE.
*/

#define LINA_SKY_DOME

#if defined(VS_BUILD)
#include <../UniformBuffers.glh>
#include <SkyboxCommon.glh>

smooth out vec3 EyeDirection;
out vec2 Vertex;
void main()
{
    mat4 inverseProjection = inverse(LINA_PROJ);
    mat3 inverseModelview = transpose(mat3(LINA_VIEW));
    vec3 unprojected = (inverseProjection * vec4(data[gl_VertexID], 0.0, 1.0)).xyz;
    EyeDirection = inverseModelview * unprojected;
    gl_Position = vec4(data[gl_VertexID], 0.0, 1.0);
    Vertex = gl_Position.xy;
}

#elif defined(FS_BUILD)
#include <../Utility.glh>
#include <../UniformBuffers.glh>
#include <../LightingData.glh>
layout (location = 0) out vec4 gPosition;		// rgb = position
layout (location = 1) out vec4 gNormal;			// rgb = normal
layout (location = 2) out vec4 gAlbedo;					
layout (location = 3) out vec4 gEmission;		    				// rgb = emission, a = workflow
layout (location = 4) out vec4 gMetallicRoughnessAOWorkflow;		// r = metallic, g = roughness, b = ao, a = workflow

smooth in vec3 EyeDirection;
in vec2 Vertex;

struct Material
{
	vec3 sunDirection;
	bool useDirectionalLight;
	float exponent;
	float radius;
	float planetOriginY;
	float groundOffset;
	vec3 waveLengthColor;
	vec3 groundColor;
	float cirrus;
	float cumulus;
};
uniform Material material;


#define CAMERA_HEIGHT 1.000001
#define INV_WAVE_LENGTH vec3( 5.60204474633241, 9.4732844379203038, 19.643802610477206 )
#define INNER_RADIUS 1.0
#define OUTER_RADIUS 1.025
#define KR 0.0025
#define SCALE_DEPTH 0.25
#define SAMPLES 2
#define G -0.99
#define GAMMA 1.0 / 2.2

// #define KM 0.0015 // sun rad
// #define GROUND_COLOR vec3( 0.37, 0.35, 0.34 )
// #define ESUN 10.0	// exponential

#define PI 3.14159265

mat2 rotate2D( float t ) {
    return mat2( cos( t ), -sin( t ), sin( t ), cos( t ) );
}

float scale( float fCos ) {
    float x = 1.0 - fCos;
    return SCALE_DEPTH * exp( -0.00287 + x * ( 0.459 + x * ( 3.83 + x * ( -6.80 + x * 5.25 ) ) ) );
}

vec2 getIntersections( vec3 eyeDir, vec3 dir, float dist2, float rad2 ) {
    float B = 2.0 * dot( eyeDir, dir );
    float C = dist2 - rad2;
    float det = max( 0.0, B * B - 4.0 * C );
    return 0.5 * vec2(
        ( -B - sqrt( det ) ),
        ( -B + sqrt( det ) )
    );
}

float getRayleighPhase( float fCos2 ) {
    return 0.75 * ( 2.0 + 0.5 * fCos2 );
}

float getMiePhase( float fCos, float fCos2, float g, float g2 ) {
    return 1.5 * ( ( 1.0 - g2 ) / ( 2.0 + g2 ) ) * ( 1.0 + fCos2 )
        / pow( 1.0 + g2 - 2.0 * g * fCos, 1.5 );
}

vec3 getSkyColor()
{
	
	// SKY
	vec3 wavelength = INV_WAVE_LENGTH * material.waveLengthColor;
	float camHeight = CAMERA_HEIGHT + material.planetOriginY * 0.01f;
    float fInnerRadius2 = INNER_RADIUS * INNER_RADIUS;
    float fOuterRadius2 = OUTER_RADIUS * OUTER_RADIUS;
    float fKrESun = KR *  material.exponent;
    float fKmESun = material.radius * 0.001f * material.exponent;
    float fKr4PI = KR * 4.0 * PI;
    float fKm4PI = material.radius * 0.001f * 4.0 * PI;
    float fScale = 1.0 / ( OUTER_RADIUS - INNER_RADIUS );
    float fScaleOverScaleDepth = fScale / SCALE_DEPTH;
    float fG2 = G * G;

	vec3 v3LightDir = material.useDirectionalLight && directionalLightExists == 1 ? normalize(-directionalLight.direction) : normalize(material.sunDirection);
    vec3 v3RayOri = vec3( 0.0, camHeight, 0.0 );
	vec3 v3RayDir =	normalize(EyeDirection);
	
    float fCameraHeight = length( v3RayOri );
    float fCameraHeight2 = fCameraHeight * fCameraHeight;

    vec2 v2InnerIsects = getIntersections( v3RayOri, v3RayDir + vec3(material.groundOffset), fCameraHeight2, fInnerRadius2 );
    vec2 v2OuterIsects = getIntersections( v3RayOri, v3RayDir + vec3(material.groundOffset), fCameraHeight2, fOuterRadius2 );
    bool isGround = 0.0 < v2InnerIsects.x;

    if ( v2OuterIsects.x == v2OuterIsects.y ) // vacuum space
        return vec3(0,0,0);

    float fNear = max( 0.0, v2OuterIsects.x );
    float fFar = isGround ? v2InnerIsects.x : v2OuterIsects.y;
    vec3 v3FarPos = v3RayOri + v3RayDir * fFar;
    vec3 v3FarPosNorm = normalize( v3FarPos );

    vec3 v3StartPos = v3RayOri + v3RayDir * fNear;
    float fStartPosHeight = length( v3StartPos );
    vec3 v3StartPosNorm = v3StartPos / fStartPosHeight;
    float fStartAngle = dot( v3RayDir, v3StartPosNorm );
    float fStartDepth = exp( fScaleOverScaleDepth * ( INNER_RADIUS - fStartPosHeight ) );
    float fStartOffset = fStartDepth * scale( fStartAngle );

    float fCameraAngle = dot( -v3RayDir, v3FarPosNorm );
    float fCameraScale = scale( fCameraAngle );
    float fCameraOffset = exp( ( INNER_RADIUS - fCameraHeight ) / SCALE_DEPTH ) * fCameraScale;

    float fTemp = scale( dot( v3FarPosNorm, v3LightDir ) ) + scale( dot( v3FarPosNorm, -v3RayDir ) );

    float fSampleLength = ( fFar - fNear ) / float( SAMPLES );
    float fScaledLength = fSampleLength * fScale;
    vec3 v3SampleDir = v3RayDir * fSampleLength;
    vec3 v3SamplePoint = v3StartPos + v3SampleDir * 0.5;

    vec3 v3FrontColor = vec3( 0.0 );
    vec3 v3Attenuate;
    for ( int i = 0; i < SAMPLES; i ++ )
        {
        float fHeight = length( v3SamplePoint );
        float fDepth = exp( fScaleOverScaleDepth * ( INNER_RADIUS - fHeight ) );
        float fLightAngle = dot( v3LightDir, v3SamplePoint ) / fHeight;
        float fCameraAngle = dot( v3RayDir, v3SamplePoint ) / fHeight;
        float fScatter = isGround
            ? fDepth * fTemp - fCameraOffset
            : ( fStartOffset + fDepth * ( scale( fLightAngle ) - scale( fCameraAngle ) ) );
        v3Attenuate = exp( -fScatter * (wavelength * fKr4PI + fKm4PI ) );
        v3FrontColor += v3Attenuate * ( fDepth * fScaledLength );
        v3SamplePoint += v3SampleDir;
    }

    v3FrontColor = clamp( v3FrontColor, 0.0, 3.0 );
    vec3 c0 = v3FrontColor * ( wavelength * fKrESun );
    vec3 c1 = v3FrontColor * fKmESun;

    if ( isGround )
    {
        vec3 v3RayleighColor = c0 + c1;
        vec3 v3MieColor = clamp( v3Attenuate, 0.0, 3.0 );
        vec3 skyColor =  vec3(1.0 - exp( -( v3RayleighColor + material.groundColor * v3MieColor ) ));
        return skyColor;
    }

	float fCos = dot( -v3LightDir, v3RayDir );
    float fCos2 = fCos * fCos;
	vec3 skyColor = vec3( getRayleighPhase( fCos2 ) * c0 + getMiePhase( fCos, fCos2, G, fG2 ) * c1);
	return skyColor;
}

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
  
vec3 getCloudsColor(vec3 skyColor)
{
	if(EyeDirection.y < 0.0f)
		return skyColor;
		
	// Cirrus Clouds
	float time = 0.0f;
	vec3 color;
	vec3 eyeDir = normalize(EyeDirection);
	
    float density = smoothstep(1.0 - material.cirrus, 1.0, fbm(eyeDir.xyz / eyeDir.y * 2.0 + time * 0.05)) * 0.3;
    color.rgb = mix(color.rgb, skyColor, density * max(eyeDir.y, 0.0));

    // Cumulus Clouds
    for (int i = 0; i < 15; i++)
    {
      float density = smoothstep(1.0 - material.cumulus, 1.0, fbm((0.7 + float(i) * 0.005) * eyeDir.xyz / eyeDir.y + time * 0.3));
      color.rgb = mix(color.rgb, skyColor * density * 5.0, min(density, 1.0) * max(eyeDir.y, 0.0));
    }
	
	return color;

}

void main()
{
	gPosition = vec4(vec3(0), 0.0f);
	gNormal = vec4(0.0f);
	gEmission = vec4(0.0f);
	gMetallicRoughnessAOWorkflow = vec4(0.0f, 0.0f, 0.0f, 2.0f); // unlit

	vec3 skyColor = getSkyColor();
	vec3 cloudColor = getCloudsColor(skyColor);
	
    gAlbedo = vec4(cloudColor,  1.0);
    gAlbedo.xyz = pow( gAlbedo.xyz, vec3( GAMMA ) );
}
#endif




