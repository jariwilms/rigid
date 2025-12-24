workspace "rigid"
	startproject   "sandbox"
	configurations { "Debug", "Release", }
	architecture   "x64"
	
	filter "configurations:Debug"
		runtime   "Debug"
		symbols   "On"
		
		targetdir "%{wks.location}/bin/debug/windows/%{prj.name}"
		objdir    "%{wks.location}/build/debug/windows/%{prj.name}"
	
	filter "configurations:Release"
		runtime   "Release"
		optimize  "On"
		
		targetdir "%{wks.location}/bin/release/windows/%{prj.name}"
		objdir    "%{wks.location}/build/release/windows/%{prj.name}"

	include "premake5.lua"

	project "sandbox"
		location         "sandbox"
		language         "C++"
		cppdialect       "C++23"
		kind             "ConsoleApp"
		staticruntime    "On"
		enablemodules    "On"
		buildstlmodules  "On"
		warnings         "Extra"
		externalwarnings "Off"
		
		includedirs {
			"rigid/source", 
			"sandbox/source", 
			"vendor/sdl/include", 
		}
		files {
			"sandbox/source/**.hpp", 
			"sandbox/source/**.cpp", 
			"sandbox/source/**.ixx", 
			
			"sandbox/source/stb_image.h", 
		}
		links {
			"rigid", 
			"sdl", 
		}
