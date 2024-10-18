workspace "R828em"
	architecture "x86_64"
	configurations { "Debug", "Release" }

project "R828em"
	kind "ConsoleApp"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
	objdir "bin/obj/%{cfg.buildcfg}"

	files { "**.h", "**.cpp" }

	filter "configurations:Debug"
        defines { "R828_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "R828_RELEASE" }
        optimize "On"