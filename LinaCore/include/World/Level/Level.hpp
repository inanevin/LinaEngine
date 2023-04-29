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

#pragma once

#ifndef Level_HPP
#define Level_HPP

#include "Core/ObjectWrapper.hpp"
#include "Data/Streams.hpp"
#include "Data/Vector.hpp"
#include "Resources/Core/CommonResources.hpp"
#include "Resources/Core/IResource.hpp"

namespace Lina
{
	class EntityWorld;
	class IEventDispatcher;
	class WorldRenderer;

	class Level : public IResource
	{
	public:
		Level(ResourceManager* rm, bool isUserManaged, const String& path, StringID sid) : IResource(rm, isUserManaged, path, sid, GetTypeID<Level>()){};
		virtual ~Level();

		void Install();
		void Uninstall();

		inline EntityWorld* GetWorld()
		{
			return m_world;
		}

		inline const Vector<ResourceIdentifier>& GetUsedResources()
		{
			return m_usedResources;
		}

		inline WorldRenderer* GetWorldRenderer() const
		{
			return m_renderer;
		}

		inline void SetWorldRenderer(WorldRenderer* renderer)
		{
			m_renderer = renderer;
		}

		inline IStream& GetLoadedRuntimeUserStream()
		{
			return m_loadedRuntimeUserStream;
		}

		inline OStream& GetOutRuntimeUserStream()
		{
			return m_runtimeUserStream;
		}

	private:
		// Inherited via IResource
		virtual void SaveToStream(OStream& stream) override;
		virtual void LoadFromStream(IStream& stream) override;

	private:
		OStream					   m_runtimeUserStream;
		IStream					   m_loadedRuntimeUserStream;
		WorldRenderer*			   m_renderer = nullptr;
		Vector<ResourceIdentifier> m_usedResources;
		EntityWorld*			   m_world = nullptr;
		IStream					   m_worldStream;
	};
} // namespace Lina

#endif
