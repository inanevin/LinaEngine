
workspace "LinaEngine"
	architecture "x64"
	
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	
IncludeDir = {}
IncludeDir["SDL"] = "LinaEngine/vendor/SDL2-2.0.9/include"
IncludeDir["GLEW"] = "LinaEngine/vendor/glew-2.1.0/include/GL"
IncludeDir["SPDLOG"] = "LinaEngine/vendor/spdlog/include"
IncludeDir["GLM"] = "LinaEngine/vendor/glm"

DLLDir = {}
DLLDir["SDL"] = "LinaEngine/vendor/SDL2-2.0.9/lib/x64"
DLLDir["GLEW"] = "LinaEngine/vendor/glew-2.1.0/bin/Release/x64"
DLLDir["LinaEngine"] = "bin/" .. outputdir .. "/LinaEngine"

LibDir = {}
LibDir["SDL"] = "LinaEngine/vendor/SDL2-2.0.9/lib/x64"
LibDir["GLEW"] = "LinaEngine/vendor/glew-2.1.0/lib/Release/x64"



project "LinaEngine"
	location "LinaEngine"
	kind "SharedLib"
	language "C++"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")
	
	pchheader "LinaPch.hpp"
	pchsource "LinaEngine/src/LinaPch.cpp"
	
	files
	{
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.SPDLOG}",
		"%{IncludeDir.GLEW}",
		"%{IncludeDir.SDL}",
		"%{IncludeDir.GLM}"
	}
	
	libdirs
	{
		"%{LibDir.SDL}",
		"%{LibDir.GLEW}"
	}
	
	links
	{
		"SDL2.lib",
		"SDL2main.lib",
		"glew32.lib",
		"glew32s.lib",
		"OpenGL32.lib"
	}
			
		filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		
		defines
		{
			"LINA_PLATFORM_WINDOWS",
			"LINA_BUILD_DLL",
			"LINA_ENABLE_ASSERTS",
			"LINA_ENABLE_LOGGING",
			"LLF_INPUTANDWINDOW_SDL",
			"LLF_GRAPHICS_OPENGL",
			"IMGUI_IMPL_OPENGL_LOADER_GLEW",
		}

	
		
	filter "configurations:Debug"
		defines "LINA_DEBUG"
		buildoptions "/MDd"
		symbols "On"
		
	filter "configurations:Release"
		defines "LINA_RELEASE"
		buildoptions "/MD"
		optimize "On"
		
	filter "configurations:Dist"
		defines "LINA_DIST"
		buildoptions "/MD"
		optimize "On"

	--[[
	filters {"system:windows", "configurations:Release"}
		buildoptions "/MT"
	--]]
	

project "Sandbox"

	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"LinaEngine/vendor/spdlog/include",
		"LinaEngine/src"
	}
	
	links
	{
		"LinaEngine"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		
		defines
		{
			"LINA_PLATFORM_WINDOWS",
			"LINA_ENABLE_LOGGING",
		}
		
		postbuildcommands
		{	
			
			("{COPY}  ../%{DLLDir.LinaEngine}/LinaEngine.dll  ../bin/" .. outputdir .. "/Sandbox"),
			("{COPY} ../%{DLLDir.SDL}/SDL2.dll ../bin/" .. outputdir .. "/Sandbox"),
			("{COPY} ../%{DLLDir.GLEW}/glew32.dll ../bin/" .. outputdir .. "/Sandbox"),
			("{COPY} ../Sandbox/Resources ../bin/" .. outputdir .. "/Sandbox/Resources")
			--- ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox"),	
			--- ("{COPY} ../%{DLLDir.SDL}/SDL2.dll ../bin/" .. outputdir .. "/Sandbox")
		}

		
	filter "configurations:Debug"
		defines "LINA_DEBUG"
		buildoptions "/MDd"
		symbols "On"
		
	filter "configurations:Release"
		defines "LINA_RELEASE"
		buildoptions "/MD"
		optimize "On"
		
	filter "configurations:Dist"
		defines "LINA_DIST"
		buildoptions "/MD"
		optimize "On"
		
				

	
