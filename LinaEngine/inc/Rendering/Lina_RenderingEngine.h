/*
Author: Inan Evin
www.inanevin.com

MIT License

Lina Engine, Copyright (c) 2018 Inan Evin

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

4.0.30319.42000
9/30/2018 4:59:45 PM

*/

#pragma once

#ifndef Lina_RenderingEngine_H
#define Lina_RenderingEngine_H

//#include<list>
#include "Rendering/Lina_Window.h"
#include "Core/Lina_ObjectHandler.h"
#include "Math/Lina_Vector3F.h"
#include "Rendering/Lights/Lina_DirectionalLight.h"
#include "Rendering/Lights/Lina_SpotLight.h"
#include <vector>

class Lina_PhongShader;
class Lina_BasicShader;
class Lina_ForwardAmbientLightShader;
class Lina_ForwardDirectionalLightShader;
class Lina_ForwardPointLightShader;
class Lina_ForwardSpotLightShader;
class Lina_Camera;
class Lina_GameCore;
class Lina_Actor;

class Lina_RenderingEngine
{

public:

	Lina_RenderingEngine();
	~Lina_RenderingEngine();

	void SetCurrentActiveCamera(Lina_Camera*);
	Lina_Camera* GetCurrentActiveCamera();
	
	Lina_Window* m_GameWindow;

	Lina_ObjectHandler eventHandler;
	Lina_PhongShader* GetPhongShader();
	Lina_BasicShader* GetBasicShader();

	float GetAspectRatio();
	int GetScreenWidth();
	int GetScreenHeight();

	Lina_Vector3F& GetAmbientLight();
	Lina_DirectionalLight& GetDirectionalLight();
	Lina_PointLight& GetPointLight();
	Lina_SpotLight& GetSpotLight();

	void AddDirectionalLight(Lina_DirectionalLight*);
	void AddSpotLight(Lina_SpotLight*);
	void AddPointLight(Lina_PointLight*);
	void ClearLights();

private:

	friend class Lina_Core;

	void Initialize(Lina_GameCore*);
	void Render();
	void CleanUp();
	void ClearScreen();

	void InitializeShaders();
	void SetTextures(bool);
	void ClearColors(float, float, float, float);
	void CreateDisplayWindow();

	Lina_BasicShader* basicShader;
	Lina_PhongShader* phongShader;
	Lina_ForwardAmbientLightShader* forwardAmbientShader;
	Lina_ForwardDirectionalLightShader* forwardDirectionalShader;
	Lina_ForwardPointLightShader* forwardPointShader;
	Lina_ForwardSpotLightShader* forwardSpotShader;

	Lina_Camera* currentActiveCamera;
	Lina_GameCore* game;

	int screenHeight = 0;
	int screenWidth = 0;
	std::string screenTitle = "";

	Lina_RenderingEngine(const Lina_RenderingEngine&) = delete;
	Lina_RenderingEngine& operator= (const Lina_RenderingEngine&) = delete;

	Lina_Vector3F ambientLight = Vector3(0.3f, 0.3f, 0.3f);
	Lina_DirectionalLight activeDirectionalLight;
	Lina_PointLight activePointLight;
	Lina_SpotLight activeSpotLight;


	std::vector<Lina_DirectionalLight*> currentDirectionalLights;
	std::vector<Lina_SpotLight*> currentSpotLights;
	std::vector<Lina_PointLight*> currentPointLights;
};


#endif