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
10/16/2018 6:11:44 PM

*/

#pragma once

#ifndef Lina_Scene_H
#define Lina_Scene_H

#include "Game/Lina_Actor.h"
#include "Game/Components/Lina_MeshRenderer.h"
#include "Core/Lina_EngineInstances.h"


class Lina_Scene
{

public:

	
	Lina_Scene();

	virtual void Initialize(Lina_EngineInstances*) = 0;
	virtual void Wake();
	virtual void Start();
	virtual void ProcessInput();
	virtual void Update();
	virtual void Render();
	virtual void Stop();
	virtual void CleanUp();
	void SetEngineInstances(Lina_EngineInstances*);

	void SetCurrentActiveCamera(Lina_Camera&);
	Lina_Camera* GetCurrentActiveCamera();


protected:

	Lina_EngineInstances* Lina;
	Lina_Actor rootActor;
	Lina_Camera* currentActiveCamera;



};


#endif