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
10/20/2018 7:39:33 PM

*/

#include "pch.h"
#include "Game/Lina_Actor.hpp"  
#include "Game//Lina_ActorComponent.hpp"


void Lina_Actor::SetEngineInstances(Lina_EngineInstances* ins)
{
	Lina = ins;
}

void Lina_Actor::AddComponent(Lina_ActorComponent* component)
{
	components.emplace_back(std::move(component));
	component->AttachToActor(*this);
}

void Lina_Actor::AddChild(Lina_Actor* child)
{
	children.push_back(child);
}

void Lina_Actor::Wake()
{
	
	for (std::vector<Lina_Actor*>::iterator it = children.begin(); it != children.end(); it++)
		(*it)->Wake();

	for (std::vector<Lina_ActorComponent*>::iterator it = components.begin(); it != components.end(); it++)
		(*it)->Wake();
}

void Lina_Actor::Start()
{
	
	for (std::vector<Lina_Actor*>::iterator it = children.begin(); it != children.end(); it++)
		(*it)->Start();

	for (std::vector<Lina_ActorComponent*>::iterator it = components.begin(); it != components.end(); it++)
		(*it)->Start();
}

void Lina_Actor::ProcessInput(float tickRate)
{

	for (std::vector<Lina_Actor*>::iterator it = children.begin(); it != children.end(); it++)
		(*it)->ProcessInput(tickRate);

	for (std::vector<Lina_ActorComponent*>::iterator it = components.begin(); it != components.end(); it++)
		(*it)->ProcessInput(tickRate);
}

void Lina_Actor::Update(float tickRate)
{

	for (std::vector<Lina_Actor*>::iterator it = children.begin(); it != children.end(); it++)
		(*it)->Update(tickRate);

	for (std::vector<Lina_ActorComponent*>::iterator it = components.begin(); it != components.end(); it++)
		(*it)->Update(tickRate);
}

void Lina_Actor::Render(Lina_Shader* shader)
{
	for (std::vector<Lina_Actor*>::iterator it = children.begin(); it != children.end(); it++)
		(*it)->Render(shader);

	for (std::vector<Lina_ActorComponent*>::iterator it = components.begin(); it != components.end(); it++)
		(*it)->Render(shader);
}

void Lina_Actor::Stop()
{

	for (std::vector<Lina_Actor*>::iterator it = children.begin(); it != children.end(); it++)
		(*it)->Stop();

	for (std::vector<Lina_ActorComponent*>::iterator it = components.begin(); it != components.end(); it++)
		(*it)->Stop();

}

void Lina_Actor::CleanUp()
{

	for (std::vector<Lina_Actor*>::iterator it = children.begin(); it != children.end(); it++)
		(*it)->CleanUp();

	for (std::vector<Lina_ActorComponent*>::iterator it = components.begin(); it != components.end(); it++)
		(*it)->CleanUp();

	// Clean component memory.
	for (std::vector<Lina_ActorComponent*>::iterator it = components.begin(); it != components.end(); ++it)
		delete(*it);

	// Clear lists.
	components.clear();
	children.clear();

}

