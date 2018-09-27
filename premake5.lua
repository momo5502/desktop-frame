require "deps/premake/cef"

workspace "desktopframe"
	configurations { "Debug", "Release" }
	platforms { "Win32", "Win64" }
	
	location "./build"
	objdir "%{wks.location}/obj"
	targetdir "%{wks.location}/bin/%{cfg.platform}/%{cfg.buildcfg}"
	buildlog "%{wks.location}/obj/%{cfg.platform}/%{cfg.buildcfg}/%{prj.name}/%{prj.name}.log"
	
	buildoptions {
		"/std:c++latest"
	}

	filter "toolset:msc*"
		buildoptions { "/utf-8", "/Zm200" }
	
	filter "platforms:*32"
		architecture "x86"
	
	filter "platforms:*64"
		architecture "x86_64"
	
	filter "platforms:Win*"
		system "windows"
		defines { "_WINDOWS" }
	
	filter {}

	flags {
		"StaticRuntime",
		"NoIncrementalLink",
		"NoMinimalRebuild",
		"MultiProcessorCompile",
		"No64BitChecks",
		"UndefinedIdentifiers"
	}
	
	largeaddressaware "on"
	editandcontinue "Off"
	warnings "Extra"
	symbols "On"

	configuration "Release*"
		defines { "NDEBUG" }
		optimize "On"
		flags {
			"FatalCompileWarnings",
			"FatalLinkWarnings",
		}

	configuration "Debug*"
		defines { "DEBUG", "_DEBUG" }
		optimize "Debug"
	
	project "desktopframe"
		kind "WindowedApp"
		language "C++"

		files {
			"./src/**.hpp",
			"./src/**.cpp",
			"./src/**.rc",
			"./src/**.manifest",
		}
		
		resincludedirs {
			"$(ProjectDir)src" -- fix for VS IDE
		}

		postbuildcommands {
			"if not exist \"%{wks.location}runtime\" mkdir \"%{wks.location}runtime\"",
			"if not exist \"%{wks.location}runtime\\%{cfg.platform}\" mkdir \"%{wks.location}runtime\\%{cfg.platform}\"",
			"if not exist \"%{wks.location}runtime\\%{cfg.platform}\\%{cfg.buildcfg}\" mkdir \"%{wks.location}runtime\\%{cfg.platform}\\%{cfg.buildcfg}\"",
			"copy /y \"$(TargetPath)\" \"%{wks.location}runtime\\%{cfg.platform}\\%{cfg.buildcfg}\"",
		}
		
		includedirs {
			"src"
		}

		pchheader "std_include.hpp"
		pchsource "src/std_include.cpp"	

		cef.import()
		
	group "Dependencies"
		cef.project()
