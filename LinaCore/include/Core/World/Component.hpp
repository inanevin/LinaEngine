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

#include "Common/SizeDefinitions.hpp"
#include "Core/World/CommonWorld.hpp"
#include "Core/Reflection/ComponentReflection.hpp"
#include "Common/StringID.hpp"
#include "Common/Data/Streams.hpp"
#include "Common/Data/Vector.hpp"
#include "Common/Memory/CommonMemory.hpp"
#include "Core/Resources/Resource.hpp"

namespace LinaGX
{
	class Input;
}
namespace Lina
{
	class Entity;
	class ReflectionClassUtility;
	class ResourceManagerV2;
	class EntityWorld;

	class Component
	{
	public:
		virtual void SaveToStream(OStream& stream) const {};
		virtual void LoadFromStream(IStream& stream){};

		virtual void Create(){};
		virtual void Destroy(){};
		virtual void Begin(){};
		virtual void End(){};
		virtual void PreTick(){};
		virtual void Tick(float delta){};
		virtual void PostTick(float delta){};

		virtual TypeID GetComponentType() = 0;

		inline Entity* GetEntity()
		{
			return m_entity;
		}

		inline Bitmask32& GetComponentFlags()
		{
			return m_flags;
		}

	protected:
		friend class EntityWorld;

		template <typename U> friend class ComponentCache;
		Component(uint32 flags = 0) : m_flags(flags){};
		virtual ~Component() = default;

		ALLOCATOR_BUCKET_MEM;
		Entity*			   m_entity			 = nullptr;
		EntityWorld*	   m_world			 = nullptr;
		ResourceManagerV2* m_resourceManager = nullptr;
		Bitmask32		   m_flags			 = 0;
		LinaGX::Input*	   m_input			 = nullptr;
	};

} // namespace Lina
