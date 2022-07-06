workspace "Olaf-assignment-1-Shadi-Jiha"
	architecture "x64"
	startproject "assignment1"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include deirectories relative to root folder
IncludeDir = {}
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["GLEW"] = "vendor/GLEW/include"
IncludeDir["glm"] = "vendor/glm"

group "Dependancies"
	include "vendor/GLFW"
	include "vendor/GLEW"
group ""

project "assignment1"
	location "assignment1"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.hpp",
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLEW}",
		"%{IncludeDir.glm}"
	}

	links
	{
		"GLFW",
		"GLEW",
		"gdi32.lib",
		"opengl32.lib",
		"shcore.lib",
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "Off"
		systemversion "latest"

		defines
		{
			"SHADO_PLATFORM_WINDOWS", "GLEW_STATIC", "SHADO_ENABLE_ASSERTS"
		}
	
		--postbuildcommands
		--{
		--	("{COPY} %{cfg.buildtarget.relpath} ../bin/" ..outputdir .. "/sandbox"),
		--}
	
	filter "configurations:Debug"
		defines {"SHADO_DEBUG", "SHADO_PROFILE"}
		symbols "On"
	
	filter "configurations:Release"
		defines "SHADO_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "SHADO_DIST"
		optimize "Full"