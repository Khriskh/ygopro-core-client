project "ocgcore"
	kind "StaticLib"

	files { "**.cc", "**.cpp", "**.c", "**.hh", "**.hpp", "**.h" }
	configuration "windows"
		includedirs { ".." }