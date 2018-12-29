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


Lina_Actor::~Lina_Actor()
{
}

void Lina_Actor::Initialize(Lina_Core* c)
{
	coreEngine = c;
	b_Initialized = true;
}

void Lina_Actor::AddComponent(Lina_ActorComponent* component)
{
	if (!b_Initialized) return;

	m_Components.emplace_back(std::move(component));
	component->AttachToActor(*this);
}

void Lina_Actor::AddChild(Lina_Actor* child)
{
	if (!b_Initialized) return;

	m_Children.push_back(child);
	child->GetTransform().SetParent(&m_Transform);
	child->Initialize(coreEngine);
}

void Lina_Actor::Wake()
{
	if (!b_Initialized) return;

	for (std::vector<Lina_Actor*>::iterator it = m_Children.begin(); it != m_Children.end(); it++)
		(*it)->Wake();

	for (std::vector<Lina_ActorComponent*>::iterator it = m_Components.begin(); it != m_Components.end(); it++)
		(*it)->Wake();
}

void Lina_Actor::Start()
{
	if (!b_Initialized) return;

	for (std::vector<Lina_Actor*>::iterator it = m_Children.begin(); it != m_Children.end(); it++)
		(*it)->Start();

	for (std::vector<Lina_ActorComponent*>::iterator it = m_Components.begin(); it != m_Components.end(); it++)
		(*it)->Start();
}

void Lina_Actor::ProcessInput(float tickRate, const Lina_InputEngine& input)
{
	if (!b_Initialized) return;

	for (std::vector<Lina_Actor*>::iterator it = m_Children.begin(); it != m_Children.end(); it++)
		(*it)->ProcessInput(tickRate, input);

	for (std::vector<Lina_ActorComponent*>::iterator it = m_Components.begin(); it != m_Components.end(); it++)
		(*it)->ProcessInput(tickRate, input);
}

void Lina_Actor::Update(float tickRate)
{
	if (!b_Initialized) return;

	for (std::vector<Lina_Actor*>::iterator it = m_Children.begin(); it != m_Children.end(); it++)
		(*it)->Update(tickRate);

	for (std::vector<Lina_ActorComponent*>::iterator it = m_Components.begin(); it != m_Components.end(); it++)
		(*it)->Update(tickRate);
}

void Lina_Actor::Render(Lina_Shader* shader)
{
	if (!b_Initialized) return;

	for (std::vector<Lina_Actor*>::iterator it = m_Children.begin(); it != m_Children.end(); it++)
		(*it)->Render(shader);

	for (std::vector<Lina_ActorComponent*>::iterator it = m_Components.begin(); it != m_Components.end(); it++)
		(*it)->Render(shader);
}

void Lina_Actor::Stop()
{
	if (!b_Initialized) return;

	for (std::vector<Lina_Actor*>::iterator it = m_Children.begin(); it != m_Children.end(); it++)
		(*it)->Stop();

	for (std::vector<Lina_ActorComponent*>::iterator it = m_Components.begin(); it != m_Components.end(); it++)
		(*it)->Stop();

}

void Lina_Actor::CleanUp()
{
	if (!b_Initialized) return;

	for (std::vector<Lina_Actor*>::iterator it = m_Children.begin(); it != m_Children.end(); it++)
		(*it)->CleanUp();

	for (std::vector<Lina_ActorComponent*>::iterator it = m_Components.begin(); it != m_Components.end(); it++)
		(*it)->CleanUp();
		

	// Clean component memory.
	for (std::vector<Lina_ActorComponent*>::iterator it = m_Components.begin(); it != m_Components.end(); ++it)
		delete(*it);

	// Clean child actor memory. TODO: CHECK IF PROPERLY DELETED
	for (std::vector<Lina_Actor*>::iterator it = m_Children.begin(); it != m_Children.end(); ++it)
		delete(*it);

	// Clear lists.
	m_Components.clear();
	m_Children.clear();

}

