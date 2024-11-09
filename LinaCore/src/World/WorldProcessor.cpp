/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Core/World/WorldProcessor.hpp"
#include "Core/Application.hpp"
#include "Core/World/EntityWorld.hpp"

namespace Lina
{

	void WorldProcessor::Initialize(Application* app)
	{
		m_app = app;
	}
	void WorldProcessor::Tick(float delta)
	{
		if (m_worlds.empty())
			return;

		if (m_worlds.size() == 1)
		{
			Process(m_worlds.at(0), delta);
		}
		else
		{
			Taskflow tf;
			tf.for_each_index(0, static_cast<int>(m_worlds.size()), 1, [&](int i) { Process(m_worlds.at(i), delta); });
			m_app->GetExecutor().RunAndWait(tf);
		}
	}

	void WorldProcessor::AddWorld(EntityWorld* world)
	{
		m_worlds.push_back(world);
	}

	void WorldProcessor::RemoveWorld(EntityWorld* world)
	{
		auto it = linatl::find_if(m_worlds.begin(), m_worlds.end(), [world](EntityWorld* w) -> bool { return w == world; });
		m_worlds.erase(it);
	}

	void WorldProcessor::Process(EntityWorld* world, float delta)
	{
		world->Tick(delta);
	}
} // namespace Lina
