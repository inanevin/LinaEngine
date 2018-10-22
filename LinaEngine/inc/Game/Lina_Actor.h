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
10/20/2018 7:39:20 PM

*/

#pragma once

#ifndef Lina_Actor_H
#define Lina_Actor_H

#include "Core/Lina_Transform.h"
#include <vector>

class Lina_ActorComponent;
class Lina_Shader;

class Lina_Actor
{

public:

	Lina_Actor();

	void AddComponent(Lina_ActorComponent*);
	void AddChild(Lina_Actor);
	void Wake();
	void Start();
	void ProcessInput(float);
	void Update(float);
	void Render(Lina_Shader*);
	void Stop();
	void CleanUp();

	Lina_Transform transform;

private:
	std::vector<Lina_ActorComponent*> components;
	std::vector<Lina_Actor> children;
};


#endif