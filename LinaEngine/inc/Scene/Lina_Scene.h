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


#include "Rendering/Lina_Mesh.h"
#include "Rendering/Lina_Shader.h"
#include "Lina_Transform.h"
#include "Lina_ObjectHandler.h"
#include "Utility/OBJ_Loader.h"

class Lina_Scene
{

public:

	Lina_Scene();
	void Wake();
	void Start();
	void ProcessInput();
	void Update();
	void Render();
	void CleanUp();

private:
	bool loadout;
	Loader objLoader;
	Lina_Matrix4F mat;
	Lina_Mesh m;
	Lina_Shader s;
	Lina_Transform transform;
};


#endif