workspace "rigid"
	startproject "run"
	architecture "x64"
	
	configurations { 
		"Debug", 
		"Release", 
	}
	
	filter "system:windows"
		systemversion "latest"
	
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



group "Application"
	project "rigid"
		location         "rigid"
		language         "C++"
		cppdialect       "C++23"
		kind             "StaticLib"
		staticruntime    "On"
		enablemodules    "On"
		buildstlmodules  "On"
		warnings         "Extra"
		externalwarnings "Off"
		
		includedirs {
			"vendor", 
			"vendor/sdl/include", 
		}
		
		files {
			"rigid/source/**.ixx", 
		}
		
		links {
			"zlib-ng", 
		}
		
		filter "system:windows"
			systemversion "latest"
			
			defines {
				"NOMINMAX", 
			}
		
		filter "configurations:Debug"
			defines   "RGD_DEBUG"
			
		filter "configurations:Release"
			defines   "RGD_RELEASE"



	project "run"
		location         "run"
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
			"vendor/sdl/include", 
		}
		files {
			"run/source/**.hpp", 
			"run/source/**.cpp", 
			"run/source/**.ixx", 
		}
		links {
			"rigid", 
			"sdl", 
		}
		
group "Vendor"
	include "vendor/sdl"
	include "vendor/zlib-ng"
group ""