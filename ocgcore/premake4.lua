project "ocgcore"
	kind "StaticLib"

	files { "**.cc", "**.cpp", "**.c", "**.hh", "**.hpp", "**.h" }
    links { "lua" }
	buildoptions { "-Wall", "-Wextra", "-pedantic" }
	
	configuration "windows"
		includedirs { ".." }