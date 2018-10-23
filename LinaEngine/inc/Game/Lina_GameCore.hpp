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
10/1/2018 5:57:03 AM

*/

#pragma once

#ifndef Lina_GameCore_HPP
#define Lina_GameCore_HPP

#include <vector>

class Lina_Scene;
class Lina_EngineInstances;
class Lina_Shader;

class Lina_GameCore
{

public:

	Lina_GameCore() {};
	~Lina_GameCore();
	virtual void Initialize(Lina_EngineInstances*) = 0;
	virtual void Wake();
	virtual void Start();
	virtual void ProcessInput(float);
	virtual void Update(float);
	virtual void Render(Lina_Shader*);
	virtual void Stop();
	virtual void CleanUp();
	void SetEngineInstances(Lina_EngineInstances*);
	void AddScene(Lina_Scene*);
	void SetActiveScene(int);

protected:

	
	Lina_EngineInstances* engineInstances;
	Lina_Scene* m_ActiveScene;
	std::vector<Lina_Scene*> m_Scenes;
	

private:
	friend class Lina_Core;
	Lina_Scene* GetActiveScene();
};


#endif