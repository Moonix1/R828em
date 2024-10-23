workspace "R828em"
	architecture "x86_64"
	configurations { "Debug", "Release" }

group "libs"
	include "libs/RASM"
group ""

project "R828em"
	kind "ConsoleApp"
	language "C++"
    cppdialect "C++20"
	targetdir "bin/%{cfg.buildcfg}"
	objdir "bin/obj/%{cfg.buildcfg}"

	files { "src/**.h", "src/**.cpp" }

	filter "configurations:Debug"
        defines { "R828_DEBUG" }
        symbols "On"
    filter "configurations:Release"
        defines { "R828_RELEASE" }
        optimize "On"