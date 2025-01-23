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

#include "Common/StringID.hpp"
#include "Common/Data/String.hpp"
#include "Common/Data/HashSet.hpp"
#include "Common/Common.hpp"
#include "Common/Data/HashSet.hpp"

namespace Lina
{
	class Resource;

#define PACKAGES_EOF UINT64_MAX - 1

#define RESOURCE_ID_ENGINE_SPACE	 UINT64_MAX - 2000
#define RESOURCE_ID_CUSTOM_SPACE	 UINT64_MAX - 10000
#define RESOURCE_ID_CUSTOM_SPACE_MAX RESOURCE_ID_ENGINE_SPACE
#define RESOURCE_ID_MAX				 RESOURCE_ID_CUSTOM_SPACE - 1

#define RESOURCE_ID_ENGINE_MODELS		 RESOURCE_ID_ENGINE_SPACE + 100
#define RESOURCE_ID_ENGINE_TEXTURES		 RESOURCE_ID_ENGINE_SPACE + 200
#define RESOURCE_ID_ENGINE_SAMPLERS		 RESOURCE_ID_ENGINE_SPACE + 300
#define RESOURCE_ID_ENGINE_SHADERS		 RESOURCE_ID_ENGINE_SPACE + 320
#define RESOURCE_ID_ENGINE_FONTS		 RESOURCE_ID_ENGINE_SPACE + 400
#define RESOURCE_ID_ENGINE_MATERIALS	 RESOURCE_ID_ENGINE_SPACE + 450
#define RESOURCE_ID_ENGINE_PHY_MATERIALS RESOURCE_ID_ENGINE_SPACE + 480

// Fonts
#define ENGINE_FONT_ROBOTO_PATH "Resources/Core/Fonts/Roboto-Regular.ttf"
#define ENGINE_FONT_ROBOTO_ID	RESOURCE_ID_ENGINE_FONTS

// Models
#define ENGINE_MODEL_CUBE_ID	  RESOURCE_ID_ENGINE_MODELS
#define ENGINE_MODEL_CYLINDER_ID  RESOURCE_ID_ENGINE_MODELS + 1
#define ENGINE_MODEL_SPHERE_ID	  RESOURCE_ID_ENGINE_MODELS + 2
#define ENGINE_MODEL_SKYCUBE_ID	  RESOURCE_ID_ENGINE_MODELS + 3
#define ENGINE_MODEL_PLANE_ID	  RESOURCE_ID_ENGINE_MODELS + 4
#define ENGINE_MODEL_SKYSPHERE_ID RESOURCE_ID_ENGINE_MODELS + 5
#define ENGINE_MODEL_CAPSULE_ID	  RESOURCE_ID_ENGINE_MODELS + 6
#define ENGINE_MODEL_QUAD_ID	  RESOURCE_ID_ENGINE_MODELS + 7

#define ENGINE_MODEL_CUBE_PATH		"Resources/Core/Models/Cube.glb"
#define ENGINE_MODEL_CYLINDER_PATH	"Resources/Core/Models/Cylinder.glb"
#define ENGINE_MODEL_SPHERE_PATH	"Resources/Core/Models/Sphere.glb"
#define ENGINE_MODEL_SKYCUBE_PATH	"Resources/Core/Models/SkyCube.glb"
#define ENGINE_MODEL_PLANE_PATH		"Resources/Core/Models/Plane.glb"
#define ENGINE_MODEL_SKYSPHERE_PATH "Resources/Core/Models/SkySphere.glb"
#define ENGINE_MODEL_CAPSULE_PATH	"Resources/Core/Models/Capsule.glb"
#define ENGINE_MODEL_QUAD_PATH		"Resources/Core/Models/Quad.glb"

// Textures
#define ENGINE_TEXTURE_EMPTY_ALBEDO_PATH			 "Resources/Core/Textures/EmptyAlbedo.png"
#define ENGINE_TEXTURE_EMPTY_NORMAL_PATH			 "Resources/Core/Textures/EmptyNormal.png"
#define ENGINE_TEXTURE_EMPTY_METALLIC_ROUGHNESS_PATH "Resources/Core/Textures/EmptyMetallicRoughness.png"
#define ENGINE_TEXTURE_EMPTY_AO_PATH				 "Resources/Core/Textures/EmptyAO.png"
#define ENGINE_TEXTURE_EMPTY_EMISSIVE_PATH			 "Resources/Core/Textures/EmptyEmissive.png"

#define ENGINE_TEXTURE_EMPTY_ALBEDO_ID			   RESOURCE_ID_ENGINE_TEXTURES + 0
#define ENGINE_TEXTURE_EMPTY_NORMAL_ID			   RESOURCE_ID_ENGINE_TEXTURES + 1
#define ENGINE_TEXTURE_EMPTY_METALLIC_ROUGHNESS_ID RESOURCE_ID_ENGINE_TEXTURES + 2
#define ENGINE_TEXTURE_EMPTY_AO_ID				   RESOURCE_ID_ENGINE_TEXTURES + 3
#define ENGINE_TEXTURE_EMPTY_EMISSIVE_ID		   RESOURCE_ID_ENGINE_TEXTURES + 4

// Samplers
#define ENGINE_SAMPLER_DEFAULT_PATH	 "DefaultSampler"
#define ENGINE_SAMPLER_GUI_PATH		 "DefaultSamplerGUI"
#define ENGINE_SAMPLER_GUI_TEXT_PATH "DefaultSamplerGUIText"
#define ENGINE_SAMPLER_DEFAULT_ID	 RESOURCE_ID_ENGINE_SAMPLERS
#define ENGINE_SAMPLER_GUI_ID		 RESOURCE_ID_ENGINE_SAMPLERS + 1
#define ENGINE_SAMPLER_GUI_TEXT_ID	 RESOURCE_ID_ENGINE_SAMPLERS + 2

// Shaders
#define ENGINE_SHADER_DEFAULT_OPAQUE_SURFACE_PATH	   "Resources/Core/Shaders/World/DefaultOpaqueSurface.linashader"
#define ENGINE_SHADER_DEFAULT_TRANSPARENT_SURFACE_PATH "Resources/Core/Shaders/World/DefaultTransparentSurface.linashader"
#define ENGINE_SHADER_DEFAULT_SKY_PATH				   "Resources/Core/Shaders/World/DefaultSky.linashader"
#define ENGINE_SHADER_DEFAULT_POSTPROCESS_PATH		   "Resources/Core/Shaders/World/DefaultPostProcess.linashader"
#define ENGINE_SHADER_LIGHTING_QUAD_PATH			   "Resources/Core/Shaders/Lighting/LightingQuad.linashader"
#define ENGINE_SHADER_WORLD_DEBUG_LINE_PATH			   "Resources/Core/Shaders/World/DebugLine.linashader"
#define ENGINE_SHADER_WORLD_DEBUG_TRIANGLE_PATH		   "Resources/Core/Shaders/World/DebugTriangle.linashader"
#define ENGINE_SHADER_GUI_DEFAULT_PATH				   "Resources/Core/Shaders/World/GUIDefault.linashader"
#define ENGINE_SHADER_GUI_TEXT_PATH					   "Resources/Core/Shaders/World/GUIText.linashader"
#define ENGINE_SHADER_GUI_SDFTEXT_PATH				   "Resources/Core/Shaders/World/GUISDFText.linashader"
#define ENGINE_SHADER_SWAPCHAIN_PATH				   "Resources/Core/Shaders/Swapchain.linashader"

#define ENGINE_SHADER_DEFAULT_OPAQUE_SURFACE_ID		 RESOURCE_ID_ENGINE_SHADERS + 0
#define ENGINE_SHADER_DEFAULT_TRANSPARENT_SURFACE_ID RESOURCE_ID_ENGINE_SHADERS + 1
#define ENGINE_SHADER_DEFAULT_SKY_ID				 RESOURCE_ID_ENGINE_SHADERS + 2
#define ENGINE_SHADER_DEFAULT_POSTPROCESS_ID		 RESOURCE_ID_ENGINE_SHADERS + 3
#define ENGINE_SHADER_LIGHTING_QUAD_ID				 RESOURCE_ID_ENGINE_SPACE + 4
#define ENGINE_SHADER_WORLD_DEBUG_LINE_ID			 RESOURCE_ID_ENGINE_SPACE + 5
#define ENGINE_SHADER_WORLD_DEBUG_TRIANGLE_ID		 RESOURCE_ID_ENGINE_SPACE + 6
#define ENGINE_SHADER_GUI_DEFAULT_ID				 RESOURCE_ID_ENGINE_SPACE + 7
#define ENGINE_SHADER_GUI_TEXT_ID					 RESOURCE_ID_ENGINE_SPACE + 8
#define ENGINE_SHADER_GUI_SDFTEXT_ID				 RESOURCE_ID_ENGINE_SPACE + 9
#define ENGINE_SHADER_SWAPCHAIN_ID					 RESOURCE_ID_ENGINE_SPACE + 10

// Materials
#define ENGINE_MATERIAL_DEFAULT_OPAQUE_OBJECT_PATH		"DefaultOpaqueMaterial"
#define ENGINE_MATERIAL_DEFAULT_TRANSPARENT_OBJECT_PATH "DefaultTransparentMaterial"
#define ENGINE_MATERIAL_DEFAULT_SKY_PATH				"DefaultSkyMaterial"
#define ENGINE_MATERIAL_GUI_DEFAULT_PATH				"GUIDefault"
#define ENGINE_MATERIAL_GUI_TEXT_PATH					"GUIText"
#define ENGINE_MATERIAL_GUI_SDFTEXT_PATH				"GUISDFText"

#define ENGINE_MATERIAL_DEFAULT_OPAQUE_OBJECT_ID	  RESOURCE_ID_ENGINE_MATERIALS
#define ENGINE_MATERIAL_DEFAULT_TRANSPARENT_OBJECT_ID RESOURCE_ID_ENGINE_MATERIALS + 1
#define ENGINE_MATERIAL_DEFAULT_SKY_ID				  RESOURCE_ID_ENGINE_MATERIALS + 2
#define ENGINE_MATERIAL_GUI_DEFAULT_ID				  RESOURCE_ID_ENGINE_MATERIALS + 3
#define ENGINE_MATERIAL_GUI_TEXT_ID					  RESOURCE_ID_ENGINE_MATERIALS + 4
#define ENGINE_MATERIAL_GUI_SDFTEXT_ID				  RESOURCE_ID_ENGINE_MATERIALS + 5

// Physical materials
#define ENGINE_PHY_MATERIAL_DEFAULT_PATH "DefaultPhysicsMaterial"
#define ENGINE_PHY_MATERIAL_DEFAULT_ID	 RESOURCE_ID_ENGINE_PHY_MATERIALS

	enum class PackageType
	{
		Default,
		Package1,
		Package2,
		PackageLevels,
	};

	enum ResourceFlags
	{
	};

	struct ResourceDef
	{
		ResourceID id		  = 0;
		String	   name		  = "";
		TypeID	   tid		  = 0;
		OStream	   customMeta = {};

		bool operator==(const ResourceDef& other) const
		{
			return id == other.id;
		}

		void SaveToStream(OStream& stream) const
		{
			stream << id << name << tid;
		}

		void LoadFromStream(IStream& stream)
		{
			stream >> id >> name >> tid;
		}
	};

	struct ResourceDefHash
	{
		std::size_t operator()(const ResourceDef& s) const noexcept
		{
			return std::hash<ResourceID>{}(s.id);
		}
	};

	struct ResourceLoadTask
	{
		Taskflow		  tf;
		Vector<Resource*> resources;
		Atomic<bool>	  isCompleted = false;
		int32			  id		  = 0;
		uint64			  startTime	  = 0;
		uint64			  endTime	  = 0;
	};

	enum class ResourceType
	{
		ExternalSource,
		EngineCreated,
	};

	typedef HashSet<ResourceDef, ResourceDefHash> ResourceDefinitionList;
	typedef HashSet<Resource*>					  ResourceList;

} // namespace Lina
