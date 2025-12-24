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
	}
	
	files {
		"rigid/source/**.ixx", 
		"rigid/vendor/**.ixx", 
	}
	
	links {
		"zlib-ng", 
	}
	
	filter "configurations:Debug"
		defines   "RGD_DEBUG"
		
	filter "configurations:Release"
		defines   "RGD_RELEASE"
	
	group "Vendor"
		include "vendor/sdl"
		include "vendor/zlib-ng"
	group ""
