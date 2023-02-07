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

#ifndef Component_HPP
#define Component_HPP

#include "Core/SizeDefinitions.hpp"
#include "World/Core/CommonWorld.hpp"
#include "Serialization/ISerializable.hpp"
#include "Event/IEventListener.hpp"
#include "Core/StringID.hpp"

namespace Lina
{
	class Entity;
	class ReflectionClassUtility;

	class Component : public ISerializable, public IEventListener
	{
	public:
		virtual void   SaveToStream(OStream& stream){};
		virtual void   LoadFromStream(IStream& stream){};
		virtual void   OnGameEvent(EGameEvent type, const Event& data) override{};
		virtual void   OnSystemEvent(ESystemEvent type, const Event& data) override{};

		virtual Bitmask32 GetGameEventMask() override
		{
			return 0;
		};

		virtual Bitmask32 GetSystemEventMask() override
		{
			return 0;
		}

		virtual Bitmask16 GetComponentMask()
		{
			return 0;
		}

		inline Entity* GetEntity()
		{
			return m_entity;
		}

		bool enabled = true;

	protected:
		template <typename U> friend class ComponentCache;
		Component()			 = default;
		virtual ~Component() = default;

	private:
		Entity* m_entity   = nullptr;
		uint32	m_entityID = 0;
	};

} // namespace Lina

#endif
