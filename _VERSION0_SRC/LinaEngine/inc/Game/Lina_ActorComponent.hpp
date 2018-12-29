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
10/20/2018 7:44:48 PM

*/

#pragma once

#ifndef Lina_ActorComponent_HPP
#define Lina_ActorComponent_HPP

class Lina_Actor;
class Lina_Shader;
class Lina_Transform;
class Lina_InputEngine;

class Lina_ActorComponent
{

public:

	Lina_ActorComponent();
	Lina_ActorComponent(const Lina_ActorComponent&);
	inline Lina_Actor* Actor() { return m_Actor; }
	Lina_Transform& GetTransform() const;

protected:
	friend class Lina_Actor;

	virtual void Wake();
	virtual void Start();
	virtual void ProcessInput(float, const Lina_InputEngine&);
	virtual void Update(float);
	virtual void Render(Lina_Shader*);
	virtual void Stop();
	virtual void CleanUp();
	virtual void AttachToActor(Lina_Actor&);
	Lina_Actor* m_Actor;

private:

	
};


#endif