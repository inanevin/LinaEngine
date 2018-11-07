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

#ifndef Lina_RenderingEngine_HPP
#define Lina_RenderingEngine_HPP

//#include<list>
#include "Rendering/Lina_Window.hpp"
#include "Core/Lina_ObjectHandler.hpp"
#include "Math/Lina_Vector.hpp"
#include "Rendering/Lina_Lighting.hpp"
#include <vector>

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

	inline void SetCurrentActiveCamera(Lina_Camera& cam) { currentActiveCamera = &cam;};


	Lina_ObjectHandler eventHandler;

	inline float GetAspectRatio() { return (float)screenWidth / (float)screenHeight; };
	inline int GetScreenWidth() { return screenWidth; };
	inline int GetScreenHeight() { return screenHeight; };
	inline Lina_Camera* GetCurrentActiveCamera() { return currentActiveCamera; };
	inline const Lina_BaseLight& GetActiveLight() const { return *activeLight; };

	Lina_Vector3F& GetAmbientLight();

	inline void AddLight(const Lina_BaseLight& light) { currentLights.push_back(&light); };
	void ClearLights();

private:

	friend class Lina_Core;

	Lina_Window* m_GameWindow;
	inline bool IsWindowClosed() { return (m_GameWindow == nullptr || m_GameWindow->IsClosed()); }

	void Initialize(Lina_GameCore*);
	void Render();
	void CleanUp();
	void ClearScreen();

	void InitializeShaders();
	void SetTextures(bool);
	void ClearColors(float, float, float, float);
	void CreateDisplayWindow();

	Lina_BasicShader* basicShader;
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

	std::vector<const Lina_BaseLight*> currentLights;
	const Lina_BaseLight* activeLight;

};


#endif