workspace "LinaEngine"
	architecture "x64"
	
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	
project "LinaEngine"
	location "LinaEngine"
	kind "SharedLib"
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
		"%{prj.name}/vendor/spdlog/include"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		
		defines
		{
			"LINA_PLATFORM_WINDOWS",
			"LINA_BUILD_DLL"
		}
		
		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}
		
	filter "configurations:Debug"
		defines "LINA_DEBUG"
		symbols "On"
		
	filter "configurations:Release"
		defines "LINA_RELEASE"
		symbols "On"
		
	filter "configurations:Debug"
		defines "LINA_DIST"
		symbols "On"

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
			"LINA_PLATFORM_WINDOWS"
		}

		
	filter "configurations:Debug"
		defines "LINA_DEBUG"
		symbols "On"
		
	filter "configurations:Release"
		defines "LINA_RELEASE"
		symbols "On"
		
	filter "configurations:Debug"
		defines "LINA_DIST"
		symbols "On"
	
