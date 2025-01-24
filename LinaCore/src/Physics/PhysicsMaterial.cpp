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

#include "Core/Physics/PhysicsMaterial.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Common/Data/Streams.hpp"

namespace Lina
{

	bool PhysicsMaterial::LoadFromFile(const String& path)
	{
		IStream stream = Serialization::LoadFromFile(path.c_str());

		if (!stream.Empty())
			LoadFromStream(stream);
		else
			return false;

		stream.Destroy();
		return true;
	}

	void PhysicsMaterial::LoadFromStream(IStream& stream)
	{
		Resource::LoadFromStream(stream);
		uint32 version = 0;
		stream >> version;
		stream >> m_restitution >> m_friction >> m_angularDamping >> m_linearDamping;
	}

	void PhysicsMaterial::SaveToStream(OStream& stream) const
	{
		Resource::SaveToStream(stream);
		stream << VERSION;
		stream << m_restitution << m_friction << m_angularDamping << m_linearDamping;
	}

	void PhysicsMaterial::GenerateHW()
	{
		LINA_ASSERT(m_hwValid == false, "");
		m_hwValid = true;
	}

	void PhysicsMaterial::DestroyHW()
	{
		LINA_ASSERT(m_hwValid, "");
		m_hwValid		= false;
		m_hwUploadValid = false;
	}

} // namespace Lina
