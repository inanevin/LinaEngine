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

#include "Core/Resources/CommonResources.hpp"

namespace Lina::Editor
{

#define RESOURCE_ID_EDITOR_SPACE	 RESOURCE_ID_ENGINE_SPACE
#define RESOURCE_ID_EDITOR_FONTS	 RESOURCE_ID_EDITOR_SPACE
#define RESOURCE_ID_EDITOR_SHADERS	 RESOURCE_ID_EDITOR_SPACE + 50
#define RESOURCE_ID_EDITOR_TEXTURES	 RESOURCE_ID_EDITOR_SPACE + 100
#define RESOURCE_ID_EDITOR_MODELS	 RESOURCE_ID_EDITOR_SPACE + 150
#define RESOURCE_ID_EDITOR_SAMPLERS	 RESOURCE_ID_EDITOR_SPACE + 200
#define RESOURCE_ID_EDITOR_MATERIALS RESOURCE_ID_EDITOR_SPACE + 250

#define EDITOR_DEF_RESOURCES_FOLDER "_Lina"

// Fonts
#define EDITOR_FONT_ROBOTO_PATH		 "Resources/Editor/Fonts/Roboto-Regular.ttf"
#define EDITOR_FONT_ROBOTO_BOLD_PATH "Resources/Editor/Fonts/Roboto-Regular.ttf"
#define EDITOR_FONT_PLAY_BIG_PATH	 "Resources/Editor/Fonts/Play-Big.ttf"
#define EDITOR_FONT_ICON_PATH		 "Resources/Editor/Fonts/EditorIcons.ttf"
#define EDITOR_FONT_PLAY_PATH		 "Resources/Editor/Fonts/Play-Regular.ttf"
#define EDITOR_FONT_PLAY_BOLD_PATH	 "Resources/Editor/Fonts/Play-Bold.ttf"

#define EDITOR_FONT_ROBOTO_ID	   RESOURCE_ID_EDITOR_FONTS
#define EDITOR_FONT_ROBOTO_BOLD_ID RESOURCE_ID_EDITOR_FONTS + 1
#define EDITOR_FONT_PLAY_BIG_ID	   RESOURCE_ID_EDITOR_FONTS + 2
#define EDITOR_FONT_ICON_ID		   RESOURCE_ID_EDITOR_FONTS + 3
#define EDITOR_FONT_PLAY_ID		   RESOURCE_ID_EDITOR_FONTS + 4
#define EDITOR_FONT_PLAY_BOLD_ID   RESOURCE_ID_EDITOR_FONTS + 5

// Shaders
#define EDITOR_SHADER_GUI_PATH				"Resources/Editor/Shaders/GUI/EditorGUI.linashader"
#define EDITOR_SHADER_LINES_PATH			"Resources/Editor/Shaders/Lines.linashader"
#define EDITOR_SHADER_DEFAULT_OBJECT_PATH	"Resources/Editor/Shaders/DefaultObject.linashader"
#define EDITOR_SHADER_DEFAULT_SKY_PATH		"Resources/Editor/Shaders/DefaultSky.linashader"
#define EDITOR_SHADER_DEFAULT_LIGHTING_PATH "Resources/Editor/Shaders/DefaultLighting.linashader"

#define EDITOR_SHADER_GUI_ID			  RESOURCE_ID_EDITOR_SHADERS
#define EDITOR_SHADER_LINES_ID			  RESOURCE_ID_EDITOR_SHADERS + 1
#define EDITOR_SHADER_DEFAULT_OBJECT_ID	  RESOURCE_ID_EDITOR_SHADERS + 2
#define EDITOR_SHADER_DEFAULT_SKY_ID	  RESOURCE_ID_EDITOR_SHADERS + 3
#define EDITOR_SHADER_DEFAULT_LIGHTING_ID RESOURCE_ID_EDITOR_SHADERS + 4

// Textures
#define EDITOR_TEXTURE_CHECKERED_PATH				 "Resources/Editor/Textures/Checkered.png"
#define EDITOR_TEXTURE_LINA_LOGO_PATH				 "Resources/Editor/Textures/LinaLogoTitle.png"
#define EDITOR_TEXTURE_EMPTY_ALBEDO_PATH			 "Resources/Editor/Textures/EmptyAlbedo.png"
#define EDITOR_TEXTURE_EMPTY_NORMAL_PATH			 "Resources/Editor/Textures/EmptyNormal.png"
#define EDITOR_TEXTURE_LINA_LOGO_BOTTOM_PATH		 "Resources/Editor/Textures/LogoBottom.png"
#define EDITOR_TEXTURE_LINA_LOGO_LEFT_PATH			 "Resources/Editor/Textures/LogoLeft.png"
#define EDITOR_TEXTURE_LINA_LOGO_RIGHT_PATH			 "Resources/Editor/Textures/LogoRight.png"
#define EDITOR_TEXTURE_PROTOTYPE_DARK_PATH			 "Resources/Editor/Textures/PrototypeDark.png"
#define EDITOR_TEXTURE_EMPTY_METALLIC_ROUGHNESS_PATH "Resources/Editor/Textures/EmptyMetallicRoughness.png"
#define EDITOR_TEXTURE_EMPTY_AO_PATH				 "Resources/Editor/Textures/EmptyAO.png"
#define EDITOR_TEXTURE_EMPTY_EMISSIVE_PATH			 "Resources/Editor/Textures/EmptyEmissive.png"

#define EDITOR_TEXTURE_CHECKERED_ID				   RESOURCE_ID_EDITOR_TEXTURES
#define EDITOR_TEXTURE_LINA_LOGO_ID				   RESOURCE_ID_EDITOR_TEXTURES + 1
#define EDITOR_TEXTURE_EMPTY_ALBEDO_ID			   RESOURCE_ID_EDITOR_TEXTURES + 2
#define EDITOR_TEXTURE_EMPTY_NORMAL_ID			   RESOURCE_ID_EDITOR_TEXTURES + 3
#define EDITOR_TEXTURE_LINA_LOGO_LEFT_ID		   RESOURCE_ID_EDITOR_TEXTURES + 4
#define EDITOR_TEXTURE_LINA_LOGO_BOTTOM_ID		   RESOURCE_ID_EDITOR_TEXTURES + 5
#define EDITOR_TEXTURE_LINA_LOGO_RIGHT_ID		   RESOURCE_ID_EDITOR_TEXTURES + 6
#define EDITOR_TEXTURE_PROTOTYPE_DARK_ID		   RESOURCE_ID_EDITOR_TEXTURES + 7
#define EDITOR_TEXTURE_EMPTY_METALLIC_ROUGHNESS_ID RESOURCE_ID_EDITOR_TEXTURES + 8
#define EDITOR_TEXTURE_EMPTY_AO_ID				   RESOURCE_ID_EDITOR_TEXTURES + 9
#define EDITOR_TEXTURE_EMPTY_EMISSIVE_ID		   RESOURCE_ID_EDITOR_TEXTURES + 10

// Materials
#define EDITOR_MATERIAL_DEFAULT_OBJ_PATH	  "DefaultObjectMaterial"
#define EDITOR_MATERIAL_DEFAULT_SKY_PATH	  "DefaultSkyMaterial"
#define EDITOR_MATERIAL_DEFAULT_LIGHTING_PATH "DefaultLightingMaterial"

#define EDITOR_MATERIAL_DEFAULT_OBJ_ID		RESOURCE_ID_EDITOR_MATERIALS
#define EDITOR_MATERIAL_DEFAULT_SKY_ID		RESOURCE_ID_EDITOR_MATERIALS + 1
#define EDITOR_MATERIAL_DEFAULT_LIGHTING_ID RESOURCE_ID_EDITOR_MATERIALS + 2

// Samplers
#define EDITOR_SAMPLER_DEFAULT_PATH "DefaultSampler"
#define EDITOR_SAMPLER_DEFAULT_ID	RESOURCE_ID_EDITOR_SAMPLERS

// Models
#define EDITOR_MODEL_CUBE_PATH		"Resources/Editor/Models/Cube.glb"
#define EDITOR_MODEL_CYLINDER_PATH	"Resources/Editor/Models/Cylinder.glb"
#define EDITOR_MODEL_SPHERE_PATH	"Resources/Editor/Models/Sphere.glb"
#define EDITOR_MODEL_SKYCUBE_PATH	"Resources/Editor/Models/SkyCube.glb"
#define EDITOR_MODEL_PLANE_PATH		"Resources/Editor/Models/Plane.glb"
#define EDITOR_MODEL_SKYSPHERE_PATH "Resources/Editor/Models/SkySphere.glb"

#define EDITOR_MODEL_CUBE_ID	  RESOURCE_ID_EDITOR_MODELS
#define EDITOR_MODEL_CYLINDER_ID  RESOURCE_ID_EDITOR_MODELS + 1
#define EDITOR_MODEL_SPHERE_ID	  RESOURCE_ID_EDITOR_MODELS + 2
#define EDITOR_MODEL_SKYCUBE_ID	  RESOURCE_ID_EDITOR_MODELS + 3
#define EDITOR_MODEL_PLANE_ID	  RESOURCE_ID_EDITOR_MODELS + 4
#define EDITOR_MODEL_SKYSPHERE_ID RESOURCE_ID_EDITOR_MODELS + 5

// Icons
#define ICON_LINA_LOGO			 "\u0041"
#define ICON_CHECK				 "\u0042"
#define ICON_CIRCLE_FILLED		 "\u0043"
#define ICON_CIRCLE				 "\u0044"
#define ICON_ARROW_LEFT			 "\u0045"
#define ICON_ARROW_RIGHT		 "\u0046"
#define ICON_ARROW_UP			 "\u0047"
#define ICON_ARROW_DOWN			 "\u0048"
#define ICON_ARROW_RECT_DOWN	 "\u0049"
#define ICON_ARROW_RECT_UP		 "\u0050"
#define ICON_ARROW_RECT_RIGHT	 "\u0051"
#define ICON_ARROW_RECT_LEFT	 "\u0052"
#define ICON_RECT_FILLED		 "\u0053"
#define ICON_ROUND_SQUARE		 "\u0054"
#define ICON_ROUND_SQUARE_FILLED "\u0055"
#define ICON_NOT_ALLOWED		 "\u0056"
#define ICON_XMARK				 "\u0057"
#define ICON_MINIMIZE			 "\u0058"
#define ICON_MAXIMIZE			 "\u0059"
#define ICON_RESTORE			 "\u0060"
#define ICON_FOLDER_CLOSED		 "\u0061"
#define ICON_FOLDER				 "\u0062"
#define ICON_FOLDER_OPEN		 "\u0063"
#define ICON_FOLDER_PLUS		 "\u0064"
#define ICON_INFO				 "\u0065"
#define ICON_ERROR				 "\u0066"
#define ICON_WARN				 "\u0067"
#define ICON_SAVE				 "\u0068"
#define ICON_SQUARE_PLUS		 "\u0069"
#define ICON_CHEVRON_LEFT		 "\u0070"
#define ICON_CHEVRON_RIGHT		 "\u0071"
#define ICON_CHEVRON_UP			 "\u0072"
#define ICON_CHEVRON_DOWN		 "\u0073"
#define ICON_PALETTE			 "\u0074"
#define ICON_L					 "\u0075"
#define ICON_COPY				 "\u0076"
#define ICON_PLUS				 "\u0077"
#define ICON_MINUS				 "\u0078"
#define ICON_BAN				 "\u0079"
#define ICON_EXPORT				 "\u007A"
#define ICON_IMPORT				 "\u007C"
#define ICON_ROTATE				 "\u007B"
#define ICON_ICONS				 "\u007D"
#define ICON_REVERT				 "\u007E"
#define ICON_SORT_AZ			 "\u004a"
#define ICON_STAR				 "\u004b"
#define ICON_TRASH				 "\u006A"

#define DOCK_DEFAULT_PERCENTAGE 0.4f
#define DOCKED_MIN_SIZE			300.0f
#define PAYLOAD_WINDOW_SID		UINT32_MAX - 2
#define RESOURCE_THUMBNAIL_SIZE 96

	enum class PanelType
	{
		Entities,
		Performance,
		World,
		WidgetEditor,
		ColorWheel,
		ResourceBrowser,
		TextureViewer,
		FontViewer,
		AudioViewer,
		ModelViewer,
		ShaderViewer,
		SamplerViewer,
		MaterialViewer,
		PhysicsMaterialViewer,
	};

	enum class PayloadType
	{
		None,
		DockedPanel,
		EntitySelectable,
		Resource,
		WidgetEditorWidget,
	};

	enum ResourceLoadTaskIDs
	{
		RLID_CORE_RES = 100,
		RLID_THUMB_RES,
	};

} // namespace Lina::Editor
