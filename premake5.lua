
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

DLLDir = {}
DLLDir["SDL"] = "LinaEngine/vendor/SDL2-2.0.9/lib/x64"
DLLDir["LinaEngine"] = "bin/" .. outputdir .. "/LinaEngine"



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
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.SDL}"
	}
	
	libdirs
	{
		"%{prj.name}/vendor/SDL2-2.0.9/lib/x64"
	}
	
	links
	{
		"SDL2.lib",
		"SDL2",
		"SDL2main.lib"
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
			"LLF_GRAPHICS_SDLOpenGL",
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
			("{COPY} ../%{DLLDir.SDL}/SDL2.dll ../bin/" .. outputdir .. "/Sandbox")
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
		
				

	
