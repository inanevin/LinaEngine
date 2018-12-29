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

#ifndef Lina_Actor_HPP
#define Lina_Actor_HPP

#include "Core/Lina_Transform.hpp"
#include <vector>

class Lina_ActorComponent;
class Lina_Core;
class Lina_InputEngine;

class Lina_Actor
{

public:

	Lina_Actor(const Lina_Vector3F& pos = Lina_Vector3F(0, 0, 0), const Lina_Quaternion& rot = Lina_Quaternion(0, 0, 0, 1), const Lina_Vector3F& scale =
		Lina_Vector3F(1, 1, 1)) : m_Transform(pos, rot, scale), coreEngine(0) {};

	virtual ~Lina_Actor();

	void AddComponent(Lina_ActorComponent*);
	void AddChild(Lina_Actor*);

	inline Lina_Transform& GetTransform() { return m_Transform; }

protected:

	friend class Lina_Scene;
	virtual void Wake();
	virtual void Start();
	virtual void ProcessInput(float, const Lina_InputEngine&);
	virtual void Update(float);
	virtual void Render(Lina_Shader*);
	virtual void Stop();
	virtual void CleanUp();

private:

	void Initialize(Lina_Core*);

	bool b_Initialized;
	Lina_Core* coreEngine;
	Lina_Transform m_Transform;
	std::vector<Lina_ActorComponent*> m_Components;
	std::vector<Lina_Actor*> m_Children;
	Lina_Actor(const Lina_Actor& rhs) {};
	void operator=(const Lina_Actor rhs) {};
};


#endif