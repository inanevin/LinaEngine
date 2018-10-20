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
#include "Rendering/Lina_PhongShader.h"
#include "Rendering/Lina_BasicShader.h"

class Lina_RenderingEngine
{

public:
	Lina_RenderingEngine() {};
	~Lina_RenderingEngine();

	void Initialize(int, int, std::string);

	void ClearScreen();
	void Render();
	void CleanUp();
	void SetTextures(bool);
	void ClearColors(float, float, float, float);
	
	std::shared_ptr<Lina_Window> m_ActiveWindow;
	Lina_ObjectHandler eventHandler;

	Lina_PhongShader* GetPhongShader();
	Lina_BasicShader* GetBasicShader();

private:

	void InitializeShaders();
	void CreateDisplayWindow(int, int, const std::string&);

	Lina_PhongShader phongShader;
	Lina_BasicShader basicShader;

	Lina_RenderingEngine(const Lina_RenderingEngine&) = delete;
	Lina_RenderingEngine& operator= (const Lina_RenderingEngine&) = delete;
	
};


#endif