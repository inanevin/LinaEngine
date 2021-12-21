/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

/*
Class: PhysicsMaterial



Timestamp: 12/20/2021 10:16:40 PM
*/

#pragma once

#ifndef PhysicsMaterial_HPP
#define PhysicsMaterial_HPP

#include "Utility/StringId.hpp"
#include <cereal/types/string.hpp>

namespace Lina::Editor
{

}

namespace Lina::Physics
{
	class PhysicsMaterial
	{
		
	public:
		
		PhysicsMaterial() {};
		~PhysicsMaterial() {};

		static PhysicsMaterial& CreateMaterial(const std::string& name, float staticFriction, float dynamicFriction, float restitution);
		static PhysicsMaterial& LoadMaterialFromFile(const std::string& path = "");
		static PhysicsMaterial& LoadMaterialFromMemory(const std::string& path, unsigned char* data, size_t dataSize);
		static PhysicsMaterial& GetMaterial(StringIDType id);
		static PhysicsMaterial& GetMaterial(const std::string& path);
		static bool MaterialExists(StringIDType id);
		static bool MaterialExists(const std::string& path);
		static void UnloadMaterialResource(StringIDType id);
		static void LoadMaterialData(PhysicsMaterial& mat, const std::string& path);
		static void SaveMaterialData(const PhysicsMaterial& mat, const std::string& path);
		static void UnloadAll();
		static std::map<StringIDType, PhysicsMaterial>& GetLoadedMaterials() { return s_loadedMaterials; }
	
		std::string GetPath() { return m_path; }
		float GetStaticFriction() { return m_staticFriction; }
		float GetDynamicFriction() { return m_dynamicFriction; }
		float GetRestitution() { return m_restitution; }

	private:

		static std::map<StringIDType, PhysicsMaterial> s_loadedMaterials;
		friend class cereal::access;

#ifdef LINA_PHYSICS_BULLET
		friend class BulletPhysicsEngine;
#elif LINA_PHYSICS_PHYSX
		friend class PhysXPhysicsEngine;
#endif

		StringIDType m_materialID = -1;
		std::string m_path = "";
		float m_staticFriction = 0.5f;
		float m_dynamicFriction = 0.5f;
		float m_restitution = 0.5f;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_staticFriction, m_dynamicFriction, m_restitution);
		}

	};
}

#endif
