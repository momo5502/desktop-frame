dependencies = {
	basePath = "./deps"
}

function dependencies.load()
	dir = path.join(dependencies.basePath, "premake/*.lua")
	deps = os.matchfiles(dir)

	for i, dep in pairs(deps) do
		dep = dep:gsub(".lua", "")
		require(dep)
	end
end

function dependencies.imports()
	for i, proj in pairs(dependencies) do
		if type(i) == 'number' then
			proj.import()
		end
	end
end

function dependencies.projects()
	for i, proj in pairs(dependencies) do
		if type(i) == 'number' then
			proj.project()
		end
	end
end

dependencies.load()

workspace "desktopframe"
	configurations { "Debug", "Release" }
	
	architecture "x86_64"
	platforms "x64"
	
	location "./build"
	objdir "%{wks.location}/obj"
	targetdir "%{wks.location}/bin/%{cfg.platform}/%{cfg.buildcfg}"
	buildlog "%{wks.location}/obj/%{cfg.platform}/%{cfg.buildcfg}/%{prj.name}/%{prj.name}.log"
	
	buildoptions {
		"/std:c++latest"
	}

	filter "toolset:msc*"
		buildoptions { "/utf-8", "/Zm200" }
	
	filter "platforms:x64"
		defines {"_WINDOWS", "WIN32"}
	filter {}
	
	filter {}

	flags {
		"NoIncrementalLink",
		"NoMinimalRebuild",
		"MultiProcessorCompile",
		"No64BitChecks"
	}
	
	largeaddressaware "on"	
	systemversion "latest"
	symbols "On"
	staticruntime "On"
	editandcontinue "Off"
	warnings "Extra"
	characterset "ASCII"

	filter "configurations:Release"
		optimize "Size"
		buildoptions {"/GL"}
		linkoptions {"/IGNORE:4702", "/LTCG"}
		defines {"NDEBUG"}
		flags {"FatalCompileWarnings"}
	filter {}

	filter "configurations:Debug"
		optimize "Debug"
		defines {"DEBUG", "_DEBUG"}
	filter {}
	
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
			"src",
			"deps/literally/include"
		}

		pchheader "std_include.hpp"
		pchsource "src/std_include.cpp"	

		dependencies.imports()
		
	group "Dependencies"
		dependencies.projects()

