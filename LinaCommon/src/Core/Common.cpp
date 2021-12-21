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

#include "Core/Common.hpp"

namespace Lina
{
	namespace Physics
	{
		std::string SIMULATION_TYPES[3] = { "None", "Static", "Dynamic" };

#ifdef LINA_PHYSICS_BULLET
		std::string COLLISION_SHAPES[4] = { "Box", "Sphere", "Cylinder", "Capsule" };
#elif LINA_PHYSICS_PHYSX
		std::string COLLISION_SHAPES[3] = { "Box", "Sphere",  "Capsule" };
#endif
	};

	namespace Resources
	{
		
		std::unordered_map<StringIDType, ResourceType> m_resourceTypeTable =
		{
			{ StringID("png"), ResourceType::Image},
			{ StringID("jpg"), ResourceType::Image },
			{ StringID("jpeg"), ResourceType::Image },
			{ StringID("tga"), ResourceType::Image },
			{ StringID("samplerparams"), ResourceType::ImageParams },
			{ StringID("hdr"), ResourceType::HDR },
			{ StringID("fbx"), ResourceType::Model },
			{ StringID("obj"), ResourceType::Model },
			{ StringID("3ds"), ResourceType::Model },
			{ StringID("modelparams"), ResourceType::ModelParams },
			{ StringID("wav"), ResourceType::Audio },
			{ StringID("mp3"), ResourceType::Audio },
			{ StringID("ogg"), ResourceType::Audio },
			{ StringID("audioparams"), ResourceType::AudioParams },
			{ StringID("mat"), ResourceType::Material },
			{ StringID("glsl"), ResourceType::GLSL },
			{ StringID("glh"), ResourceType::GLH },
			{ StringID("spv"), ResourceType::SPIRV },
			{ StringID("ttf"), ResourceType::Font },
			{ StringID("otf"), ResourceType::Font },
			{ StringID("phymat"), ResourceType::PhysicsMaterial },
		};

		void RegisterUserDefinedResource(const std::string& extension)
		{
			m_resourceTypeTable[StringID(extension.c_str())] = ResourceType::UserAsset;
		}

		ResourceType GetResourceType(const std::string& extension)
		{
			return m_resourceTypeTable[StringID(extension.c_str())];
		}
	}

	std::string LogLevelAsString(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::Critical:
			return "Critical";
		case LogLevel::Debug:
			return "Debug";
		case LogLevel::Error:
			return "Error";
		case LogLevel::Info:
			return "Info";
		case LogLevel::None:
			return "None";
		case LogLevel::Trace:
			return "Trace";
		case LogLevel::Warn:
			return "Warn";
		default:
			return "";
		}
	}
}