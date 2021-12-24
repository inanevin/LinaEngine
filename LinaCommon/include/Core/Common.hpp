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
Class: Common

Common macros are defined here.

Timestamp: 4/7/2019 3:29:18 PM
*/

#pragma once

#ifndef Common_HPP
#define Common_HPP

#include "Core/SizeDefinitions.hpp"
#include "Utility/StringId.hpp"
#include "Math/Color.hpp"
#include <unordered_map>
#include <string>

namespace Lina
{
	namespace Physics
	{
		extern std::string SIMULATION_TYPES[3];

		enum class SimulationType : uint8
		{
			None = 0,
			Static = 1,
			Dynamic = 2,
		};

#ifdef LINA_PHYSICS_BULLET
		extern std::string COLLISION_SHAPES[4];

		enum class CollisionShape : uint8
		{
			Box = 0,
			Sphere = 1,
			Cylinder = 2,
			Capsule = 3,
			ConvexMesh = 4,
		};
#elif LINA_PHYSICS_PHYSX
		extern std::string COLLISION_SHAPES[4];

		enum class CollisionShape : uint8
		{
			Box = 0,
			Sphere = 1,
			Capsule = 2,
			ConvexMesh = 3,
		};
#endif
	}

	namespace Graphics
	{

	}
	namespace Resources
	{
		enum class ResourceType
		{
			Unknown = 0,
			Model = 1,
			ModelAssetData = 2,
			Image = 3,
			ImageData = 4,
			HDR = 5,
			Audio = 6,
			AudioData = 7,
			Material = 8,
			GLSL = 9,
			GLH,
			SPIRV = 11,
			Font = 12,
			PhysicsMaterial = 13,
			UserAsset = 20
		};

		/// <summary>
		/// Register a user defined resource to be picked-up & request to load by the resource manager.
		/// Pass in the extension without '.'
		/// </summary>
		extern void RegisterUserDefinedResource(const std::string& extension);
		extern std::unordered_map<StringIDType, ResourceType> m_resourceTypeTable;
		extern ResourceType GetResourceType(const std::string& extension);
	}

	enum class WindowState
	{
		Normal = 0,
		Maximized = 1,
		Iconified = 2
	};

	enum class InputAction
	{
		Pressed,
		Released,
		Repeated
	};

	struct LevelData
	{
		std::string m_skyboxMaterialPath = "";
		int m_skyboxMaterialID = -1;
		int m_selectedSkyboxMatID = -1;
		std::string m_selectedSkyboxMatPath = "";
		Color m_ambientColor = Color(0);

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_skyboxMaterialPath, m_selectedSkyboxMatPath, m_ambientColor);
		}
	};


	/* Struct containing basic data about window properties. */
	struct WindowProperties
	{
		std::string m_title;
		int m_width;
		int m_height;
		unsigned int m_xPos = 0;
		unsigned int m_yPos = 0;
		unsigned int m_xPosBeforeMaximize = 0;
		unsigned int m_yPosBeforeMaximize = 0;
		unsigned int m_widthBeforeMaximize = 256;
		unsigned int m_heightBeforeMaximize = 256;
		unsigned int m_workingAreaWidth = 256;
		unsigned int m_workingAreaHeight = 256;
		int m_vsync = 0;
		bool m_decorated = true;
		bool m_resizable = true;
		bool m_fullscreen = false;
		int m_msaaSamples = 4;
		WindowState m_windowState;

		WindowProperties()
		{
			m_title = "Lina Engine";
			m_width = 1440;
			m_height = 900;
		}

		WindowProperties(const std::string& title, unsigned int width, unsigned int height)
		{
			m_title = title;
			m_width = width;
			m_height = height;
		}
	};

	enum LogLevel 
	{
		None = 1 << 0,
		Debug = 1 << 1,
		Info = 1 << 2,
		Critical = 1 << 3,
		Error = 1 << 4,
		Trace = 1 << 5,
		Warn = 1 << 6
	};

	extern std::string LogLevelAsString(LogLevel level);

	enum class ApplicationMode
	{
		Editor = 1 << 0,
		Standalone = 1 << 1
	};

	struct ApplicationInfo
	{
		// Bundle name that is used to load resources package on Standalone builds.
		std::string m_bundleName = "";
		
		// App Info
		const char* m_appName = "";
		int m_appMajor = 0;
		int m_appMinor = 0;
		int m_appPatch = 0;
		
		ApplicationMode m_appMode = ApplicationMode::Editor;
		WindowProperties m_windowProperties = WindowProperties();
	};
}

/****************************************** OPTIONS ******************************************/

#ifdef LINA_COMPILER_MSVC
#define FORCEINLINE __forceinline
#elif defined(LINA_COMPILER_GCC) || defined(LINA_COMPILER_CLANG)
#define FORCEINLINE inline __attribute__ ((always_inline))
#else
#define FORCEINLINE inline
#endif

#define BIT(x) (1 << x)
#define INVALID_VALUE 0xFFFFFFFF
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define LINA_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#define NULL_COPY_AND_ASSIGN(T) \
	T(const T& other) {(void)other;} \
	void operator=(const T& other) { (void)other; }

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete;

#define DISALLOW_COPY_ASSIGN_MOVE(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete; \
	TypeName(TypeName&&) = delete; \
  TypeName& operator=(TypeName&&) = delete; 

#define DISALLOW_COPY_ASSIGN_NEW(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete; \
  void* operator new(std::size_t) = delete;

#define DISALLOW_COPY_ASSIGN_NEW_MOVE(TypeName) \
  TypeName(const TypeName&) = delete;      \
  TypeName(TypeName&&) = delete; \
  TypeName& operator=(TypeName&&) = delete; \
  void operator=(const TypeName&) = delete; \
  void* operator new(std::size_t) = delete;

#endif