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

namespace Lina::Editor
{

#define RESOURCE_ID_EDITOR_SPACE		 RESOURCE_ID_ENGINE_SPACE + 1000
#define RESOURCE_ID_EDITOR_FONTS		 RESOURCE_ID_EDITOR_SPACE
#define RESOURCE_ID_EDITOR_SHADERS		 RESOURCE_ID_EDITOR_SPACE + 50
#define RESOURCE_ID_EDITOR_TEXTURES		 RESOURCE_ID_EDITOR_SPACE + 100
#define RESOURCE_ID_EDITOR_MODELS		 RESOURCE_ID_EDITOR_SPACE + 150
#define RESOURCE_ID_EDITOR_SAMPLERS		 RESOURCE_ID_EDITOR_SPACE + 200
#define RESOURCE_ID_EDITOR_MATERIALS	 RESOURCE_ID_EDITOR_SPACE + 250
#define RESOURCE_ID_EDITOR_PHY_MATERIALS RESOURCE_ID_EDITOR_SPACE + 300

// Fonts
#define EDITOR_FONT_ROBOTO_PATH		   "Resources/Editor/Fonts/Roboto-Regular.ttf"
#define EDITOR_FONT_ROBOTO_BOLD_PATH   "Resources/Editor/Fonts/Roboto-Regular.ttf"
#define EDITOR_FONT_PLAY_BIG_PATH	   "Resources/Editor/Fonts/Play-Big.ttf"
#define EDITOR_FONT_ICON_PATH		   "Resources/Editor/Fonts/EditorIcons.ttf"
#define EDITOR_FONT_PLAY_PATH		   "Resources/Editor/Fonts/Play-Regular.ttf"
#define EDITOR_FONT_PLAY_BOLD_PATH	   "Resources/Editor/Fonts/Play-Bold.ttf"
#define EDITOR_FONT_PLAY_VERY_BIG_PATH "Resources/Editor/Fonts/Play-Big.ttf"

#define EDITOR_FONT_ROBOTO_ID		 RESOURCE_ID_EDITOR_FONTS
#define EDITOR_FONT_ROBOTO_BOLD_ID	 RESOURCE_ID_EDITOR_FONTS + 1
#define EDITOR_FONT_PLAY_BIG_ID		 RESOURCE_ID_EDITOR_FONTS + 2
#define EDITOR_FONT_ICON_ID			 RESOURCE_ID_EDITOR_FONTS + 3
#define EDITOR_FONT_PLAY_ID			 RESOURCE_ID_EDITOR_FONTS + 4
#define EDITOR_FONT_PLAY_BOLD_ID	 RESOURCE_ID_EDITOR_FONTS + 5
#define EDITOR_FONT_PLAY_VERY_BIG_ID RESOURCE_ID_EDITOR_FONTS + 6

// Shaders
#define EDITOR_SHADER_GUI_DEFAULT_PATH				   "Resources/Editor/Shaders/GUI/GUIDefault.linashader"
#define EDITOR_SHADER_GUI_COLOR_WHEEL_PATH			   "Resources/Editor/Shaders/GUI/ColorWheel.linashader"
#define EDITOR_SHADER_GUI_HUE_DISPLAY_PATH			   "Resources/Editor/Shaders/GUI/HueDisplay.linashader"
#define EDITOR_SHADER_GUI_SDF_TEXT_PATH				   "Resources/Editor/Shaders/GUI/SDFText.linashader"
#define EDITOR_SHADER_GUI_TEXT_PATH					   "Resources/Editor/Shaders/GUI/Text.linashader"
#define EDITOR_SHADER_GUI_GLITCH_PATH				   "Resources/Editor/Shaders/GUI/GUIGlitch.linashader"
#define EDITOR_SHADER_LINES_PATH					   "Resources/Editor/Shaders/Lines.linashader"
#define EDITOR_SHADER_DEFAULT_OPAQUE_SURFACE_PATH	   "Resources/Editor/Shaders/DefaultOpaqueSurface.linashader"
#define EDITOR_SHADER_DEFAULT_TRANSPARENT_SURFACE_PATH "Resources/Editor/Shaders/DefaultTransparentSurface.linashader"
#define EDITOR_SHADER_DEFAULT_SKY_PATH				   "Resources/Editor/Shaders/DefaultSky.linashader"
#define EDITOR_SHADER_GUI_DISPLAYTARGET_PATH		   "Resources/Editor/Shaders/GUI/DisplayTarget.linashader"
#define EDITOR_SHADER_DEFAULT_POSTPROCESS_PATH		   "Resources/Editor/Shaders/DefaultPostProcess.linashader"
#define EDITOR_SHADER_WORLD_GRID_PATH				   "Resources/Editor/Shaders/World/Grid.linashader"
#define EDITOR_SHADER_WORLD_GIZMO_PATH				   "Resources/Editor/Shaders/World/Gizmo.linashader"
#define EDITOR_SHADER_WORLD_OUTLINE_FULLSCREEN_PATH	   "Resources/Editor/Shaders/World/OutlineFullscreen.linashader"
#define EDITOR_SHADER_WORLD_LINE3D_PATH				   "Resources/Editor/Shaders/World/Line3D.linashader"
#define EDITOR_SHADER_WORLD_LVG3D_PATH				   "Resources/Editor/Shaders/World/LinaVG3D.linashader"
#define EDITOR_SHADER_WORLD_SAMPLE_PATH				   "Resources/Editor/Shaders/World/WorldSample.linashader"
#define EDITOR_SHADER_WORLD_DEPTH_SAMPLE_PATH		   "Resources/Editor/Shaders/World/WorldSampleDepth.linashader"
#define EDITOR_SHADER_WORLD_GIZMO_ROTATE_PATH		   "Resources/Editor/Shaders/World/GizmoRotate.linashader"
#define EDITOR_SHADER_WORLD_ORIENT_GIZMO_PATH		   "Resources/Editor/Shaders/World/OrientationGizmo.linashader"
#define EDITOR_SHADER_GUI_SWAPCHAIN_PATH			   "Resources/Editor/Shaders/GUI/Swapchain.linashader"
#define EDITOR_SHADER_WORLD_BILLBOARD_SDF_PATH		   "Resources/Editor/Shaders/World/BillboardSDF.linashader"
#define EDITOR_SHADER_WORLD_DEBUG_PHYSICS_PATH		   "Resources/Editor/Shaders/World/PhysicsDebug.linashader"

#define EDITOR_SHADER_GUI_DEFAULT_ID				 RESOURCE_ID_EDITOR_SHADERS
#define EDITOR_SHADER_GUI_COLOR_WHEEL_ID			 RESOURCE_ID_EDITOR_SHADERS + 1
#define EDITOR_SHADER_GUI_HUE_DISPLAY_ID			 RESOURCE_ID_EDITOR_SHADERS + 2
#define EDITOR_SHADER_GUI_SDF_TEXT_ID				 RESOURCE_ID_EDITOR_SHADERS + 3
#define EDITOR_SHADER_GUI_TEXT_ID					 RESOURCE_ID_EDITOR_SHADERS + 4
#define EDITOR_SHADER_LINES_ID						 RESOURCE_ID_EDITOR_SHADERS + 5
#define EDITOR_SHADER_DEFAULT_OPAQUE_SURFACE_ID		 RESOURCE_ID_EDITOR_SHADERS + 6
#define EDITOR_SHADER_DEFAULT_TRANSPARENT_SURFACE_ID RESOURCE_ID_EDITOR_SHADERS + 7
#define EDITOR_SHADER_DEFAULT_SKY_ID				 RESOURCE_ID_EDITOR_SHADERS + 8
#define EDITOR_SHADER_GUI_DISPLAYTARGET_ID			 RESOURCE_ID_EDITOR_SHADERS + 9
#define EDITOR_SHADER_DEFAULT_POSTPROCESS_ID		 RESOURCE_ID_EDITOR_SHADERS + 10
#define EDITOR_SHADER_WORLD_GRID_ID					 RESOURCE_ID_EDITOR_SHADERS + 11
#define EDITOR_SHADER_GUI_GLITCH_ID					 RESOURCE_ID_EDITOR_SHADERS + 12
#define EDITOR_SHADER_WORLD_GIZMO_ID				 RESOURCE_ID_EDITOR_SHADERS + 13
#define EDITOR_SHADER_WORLD_DEPRECATED				 RESOURCE_ID_EDITOR_SHADERS + 14
#define EDITOR_SHADER_WORLD_OUTLINE_FULLSCREEN_ID	 RESOURCE_ID_EDITOR_SHADERS + 15
#define EDITOR_SHADER_WORLD_LINE3D_ID				 RESOURCE_ID_EDITOR_SHADERS + 16
#define EDITOR_SHADER_WORLD_LVG3D_ID				 RESOURCE_ID_EDITOR_SHADERS + 17
#define EDITOR_SHADER_WORLD_SAMPLE_ID				 RESOURCE_ID_EDITOR_SHADERS + 18
#define EDITOR_SHADER_WORLD_DEPTH_SAMPLE_ID			 RESOURCE_ID_EDITOR_SHADERS + 19
#define EDITOR_SHADER_WORLD_GIZMO_ROTATE_ID			 RESOURCE_ID_EDITOR_SHADERS + 20
#define EDITOR_SHADER_WORLD_ORIENT_GIZMO_ID			 RESOURCE_ID_EDITOR_SHADERS + 21
#define EDITOR_SHADER_GUI_SWAPCHAIN_ID				 RESOURCE_ID_EDITOR_SHADERS + 22
#define EDITOR_SHADER_WORLD_BILLBOARD_SDF_ID		 RESOURCE_ID_EDITOR_SHADERS + 23
#define EDITOR_SHADER_WORLD_DEBUG_PHYSICS_ID		 RESOURCE_ID_EDITOR_SHADERS + 24

// Textures
#define EDITOR_TEXTURE_CHECKERED_PATH		 "Resources/Editor/Textures/Checkered.png"
#define EDITOR_TEXTURE_LINA_LOGO_PATH		 "Resources/Editor/Textures/LinaLogoTitle.png"
#define EDITOR_TEXTURE_LINA_LOGO_BOTTOM_PATH "Resources/Editor/Textures/LogoBottom.png"
#define EDITOR_TEXTURE_LINA_LOGO_LEFT_PATH	 "Resources/Editor/Textures/LogoLeft.png"
#define EDITOR_TEXTURE_LINA_LOGO_RIGHT_PATH	 "Resources/Editor/Textures/LogoRight.png"
#define EDITOR_TEXTURE_PROTOTYPE_DARK_PATH	 "Resources/Editor/Textures/PrototypeDark.png"

#define EDITOR_TEXTURE_CHECKERED_ID		   RESOURCE_ID_EDITOR_TEXTURES
#define EDITOR_TEXTURE_LINA_LOGO_ID		   RESOURCE_ID_EDITOR_TEXTURES + 1
#define EDITOR_TEXTURE_LINA_LOGO_LEFT_ID   RESOURCE_ID_EDITOR_TEXTURES + 2
#define EDITOR_TEXTURE_LINA_LOGO_BOTTOM_ID RESOURCE_ID_EDITOR_TEXTURES + 3
#define EDITOR_TEXTURE_LINA_LOGO_RIGHT_ID  RESOURCE_ID_EDITOR_TEXTURES + 4
#define EDITOR_TEXTURE_PROTOTYPE_DARK_ID   RESOURCE_ID_EDITOR_TEXTURES + 5

// Materials
#define EDITOR_MATERIAL_DEFAULT_OPAQUE_OBJECT_PATH		"DefaultOpaqueMaterial"
#define EDITOR_MATERIAL_DEFAULT_TRANSPARENT_OBJECT_PATH "DefaultTransparentMaterial"
#define EDITOR_MATERIAL_DEFAULT_SKY_PATH				"DefaultSkyMaterial"
#define EDITOR_MATERIAL_DEFAULT_LIGHTING_PATH			"DefaultLightingMaterial"

#define EDITOR_MATERIAL_DEFAULT_OPAQUE_OBJECT_ID	  RESOURCE_ID_EDITOR_MATERIALS
#define EDITOR_MATERIAL_DEFAULT_TRANSPARENT_OBJECT_ID RESOURCE_ID_EDITOR_MATERIALS + 1
#define EDITOR_MATERIAL_DEFAULT_SKY_ID				  RESOURCE_ID_EDITOR_MATERIALS + 2

// Samplers
#define EDITOR_SAMPLER_DEFAULT_PATH "DefaultSampler"
#define EDITOR_SAMPLER_DEFAULT_ID	RESOURCE_ID_EDITOR_SAMPLERS

	// Models

#define EDITOR_MODEL_GIZMO_TRANSLATE_PATH		 "Resources/Editor/Models/GizmoTranslate.glb"
#define EDITOR_MODEL_GIZMO_ROTATE_PATH			 "Resources/Editor/Models/GizmoRotate.glb"
#define EDITOR_MODEL_GIZMO_SCALE_PATH			 "Resources/Editor/Models/GizmoScale.glb"
#define EDITOR_MODEL_GIZMO_TRANSLATE_CENTER_PATH "Resources/Engine/Models/Sphere.glb"
#define EDITOR_MODEL_GIZMO_SCALE_CENTER_PATH	 "Resources/Engine/Models/Cube.glb"
#define EDITOR_MODEL_GIZMO_ROTATE_FULL_PATH		 "Resources/Editor/Models/GizmoRotateFull.glb"
#define EDITOR_MODEL_GIZMO_ORIENTATION_PATH		 "Resources/Editor/Models/GizmoOrientation.glb"

#define EDITOR_MODEL_GIZMO_TRANSLATE_ID		   RESOURCE_ID_EDITOR_MODELS + 0
#define EDITOR_MODEL_GIZMO_ROTATE_ID		   RESOURCE_ID_EDITOR_MODELS + 1
#define EDITOR_MODEL_GIZMO_SCALE_ID			   RESOURCE_ID_EDITOR_MODELS + 2
#define EDITOR_MODEL_GIZMO_TRANSLATE_CENTER_ID RESOURCE_ID_EDITOR_MODELS + 3
#define EDITOR_MODEL_GIZMO_SCALE_CENTER_ID	   RESOURCE_ID_EDITOR_MODELS + 4
#define EDITOR_MODEL_GIZMO_ROTATE_FULL_ID	   RESOURCE_ID_EDITOR_MODELS + 5
#define EDITOR_MODEL_GIZMO_ORIENTATION_ID	   RESOURCE_ID_EDITOR_MODELS + 6

#define EDITOR_PHY_MATERIAL_DEFAULT_PATH "DefaultPhysicsMaterial"
#define EDITOR_PHY_MATERIAL_DEFAULT_ID	 RESOURCE_ID_EDITOR_PHY_MATERIALS

// Icons
#define ICON_LINA_LOGO			  "\u0041"
#define ICON_CHECK				  "\u0042"
#define ICON_CIRCLE_FILLED		  "\u0043"
#define ICON_CIRCLE				  "\u0044"
#define ICON_ARROW_LEFT			  "\u0045"
#define ICON_ARROW_RIGHT		  "\u0046"
#define ICON_ARROW_UP			  "\u0047"
#define ICON_ARROW_DOWN			  "\u0048"
#define ICON_ARROW_RECT_DOWN	  "\u0049"
#define ICON_ARROW_RECT_UP		  "\u0050"
#define ICON_ARROW_RECT_RIGHT	  "\u0051"
#define ICON_ARROW_RECT_LEFT	  "\u0052"
#define ICON_RECT_FILLED		  "\u0053"
#define ICON_ROUND_SQUARE		  "\u0054"
#define ICON_ROUND_SQUARE_FILLED  "\u0055"
#define ICON_NOT_ALLOWED		  "\u0056"
#define ICON_XMARK				  "\u0057"
#define ICON_MINIMIZE			  "\u0058"
#define ICON_MAXIMIZE			  "\u0059"
#define ICON_RESTORE			  "\u0060"
#define ICON_FOLDER_CLOSED		  "\u0061"
#define ICON_FOLDER				  "\u0062"
#define ICON_FOLDER_OPEN		  "\u0063"
#define ICON_FOLDER_PLUS		  "\u0064"
#define ICON_CHECK_CIRCLE		  "\u004c"
#define ICON_EXCLAMATION_CIRCLE	  "\u004d"
#define ICON_XMARK_CIRCLE		  "\u005d"
#define ICON_EXCLAMATION_TRIANGLE "\u004e"
#define ICON_INFO_CIRCLE		  "\u004f"
#define ICON_SAVE				  "\u0068"
#define ICON_SQUARE_PLUS		  "\u0069"
#define ICON_CHEVRON_LEFT		  "\u0070"
#define ICON_CHEVRON_RIGHT		  "\u0071"
#define ICON_CHEVRON_UP			  "\u0072"
#define ICON_CHEVRON_DOWN		  "\u0073"
#define ICON_PALETTE			  "\u0074"
#define ICON_L					  "\u0075"
#define ICON_COPY				  "\u0076"
#define ICON_PLUS				  "\u0077"
#define ICON_MINUS				  "\u0078"
#define ICON_BAN				  "\u0079"
#define ICON_EXPORT				  "\u007A"
#define ICON_IMPORT				  "\u007C"
#define ICON_ROTATE				  "\u007B"
#define ICON_ICONS				  "\u007D"
#define ICON_REVERT				  "\u007E"
#define ICON_SORT_AZ			  "\u004a"
#define ICON_STAR				  "\u004b"
#define ICON_TRASH				  "\u006A"
#define ICON_FILE				  "\u0067"
#define ICON_FILE_PEN			  "\u0066"
#define ICON_EDIT_PEN			  "\u0065"
#define ICON_GLOBE				  "\u005A"
#define ICON_GITHUB				  "\u005B"
#define ICON_DISCORD			  "\u005C"
#define ICON_SEARCH				  "\u006B"
#define ICON_TERMINAL			  "\u006C"
#define ICON_FILTER				  "\u006D"
#define ICON_CUBE				  "\u007F"
#define ICON_GAUGE				  "\u0020"
#define ICON_MEMORY				  "\u0021"
#define ICON_MOVE				  "\u0022"
#define ICON_SCALE				  "\u0023"
#define ICON_MAGNET				  "\u0024"
#define ICON_EYE				  "\u0025"
#define ICON_EYE_SLASH			  "\u0026"
#define ICON_MAGNET_SLASH		  "\u0028"
#define ICON_ARROW_LOCATION		  "\u0027"
#define ICON_GRID				  "\u0029"
#define ICON_SITEMAP			  "\u002A"
#define ICON_ROTATE_LEFT		  "\u002B"
#define ICON_PLAY				  "\u002C"
#define ICON_SLIDERS			  "\u002D"
#define ICON_VIDEO				  "\u002E"
#define ICON_CUBES_STACKED		  "\u002F"
#define ICON_CODE				  "\u0030"
#define ICON_LIGHTBULD			  "\u0031"
#define ICON_SPOTLIGHT			  "\u0032"
#define ICON_SUN				  "\u0033"

#define DOCK_DEFAULT_PERCENTAGE	  0.4f
#define DOCKED_MIN_SIZE			  300.0f
#define PAYLOAD_WINDOW_SID		  UINT32_MAX - 2
#define RESOURCE_THUMBNAIL_SIZE	  96
#define EDITOR_WORLD_MSAA_SAMPLES 4

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
		Log,
		Details,
		GenericSelector,
		ProjectSettings,
	};

	enum class ResourceDirectoryType
	{
		Default = 0,
		LinaAssetsFolder,
		EngineResource,
	};

	enum ResourceDirectoryMask
	{
		RDM_NONE = 1 << 0,
	};

	enum WindowFlags
	{
		WINDOW_FLAGS_NONE				= 1 << 0,
		WINDOW_FLAGS_FOCUS_LOST_DESTROY = 1 << 1,
	};

} // namespace Lina::Editor
