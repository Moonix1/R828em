project "RASM"
	kind "ConsoleApp"
	language "C++"
    cppdialect "C++20"
	targetdir "bin/%{cfg.buildcfg}"
	objdir "bin/obj/%{cfg.buildcfg}"

	files {
		"src/**.h",
		"src/**.cpp",
	}

	includedirs {
		"src",
		"libs/spdlog/include"
	}

	filter "configurations:Debug"
        defines { "RASM_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "RASM_RELEASE" }
        optimize "On"