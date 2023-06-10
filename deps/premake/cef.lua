cef = {
	versionShort = "114.2.10",
	versionLong = "cef_binary_114.2.10+g398e3c3+chromium-114.0.5735.110_windows64",
	source = "deps/cef"
}

function cef.import()
	filter {"kind:not StaticLib" }
	links {
		"cef",
		"cef_sandbox",
		"libcef",
		"comctl32.lib",
		"gdi32.lib",
		"rpcrt4.lib",
		"shlwapi.lib",
		"ws2_32.lib",
		"Advapi32.lib",
		"dbghelp.lib",
		"Delayimp.lib",
		"ntdll.lib",
		"OleAut32.lib",
		"PowrProf.lib",
		"Propsys.lib",
		"psapi.lib",
		"SetupAPI.lib",
		"Shell32.lib",
		"Shcore.lib",
		"Userenv.lib",
		"version.lib",
		"wbemuuid.lib",
		"WindowsApp.lib",
		"winmm.lib",
	}
	linkoptions { "/DELAYLOAD:libcef.dll" }
	filter {}
	
	cef.includes()
end

function cef.includes()
	includedirs { cef.source }
	defines {
		"WRAPPING_CEF_SHARED",
		"NOMINMAX",
		"USING_CEF_SHARED",
	}
	
	filter { "Release" }
		libdirs { path.join(cef.source, "Release") }
	filter { "Debug" }
		libdirs { path.join(cef.source, "Debug") }
		defines {
			"_ITERATOR_DEBUG_LEVEL=0",
			"_HAS_ITERATOR_DEBUGGING=0",
		}
	filter {}
end

function cef.install()
	if os.host() == "windows" then
		local result = os.executef("powershell -c \"Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process; %s %s\"", ".\\scripts\\get-cef.ps1", cef.versionLong)
		return result == true
	else
		premake.error(string.format("Your OS does not support automatic CEF installation.\n"
			.. "Please download CEF version '%s' yourself and place it in 'deps/cef'.\n"
			.. "Afterwards create a file 'deps/cef/.launcher_version.txt' with content '%s'.",
			cef.versionShort, cef.versionShort
		))
	end
	return true
end

function cef.checkVersion()
	local versionFile = path.join(cef.source, ".launcher_version.txt")
	local installedVersion = io.readfile(versionFile)

	if installedVersion ~= cef.versionShort then
		print("CEF dependency outdated. Attempting to install new version.")
		if cef.install() then
			io.writefile(versionFile, cef.versionShort)
		else
			premake.error("Failed to install CEF.")
		end
	end
end

function cef.project()
	cef.checkVersion()

	project "cef"
		language "C++"

		cef.includes()
		files
		{
			path.join(cef.source, "libcef_dll/**.h"),
			path.join(cef.source, "libcef_dll/**.cc"),
		}

		postbuildcommands {
			"mkdir \"%{wks.location}runtime/%{cfg.buildcfg}/cef/\" 2> nul",
			"mkdir \"%{wks.location}runtime/%{cfg.buildcfg}/cef/locales/\" 2> nul",
			"copy /y \"%{wks.location}..\\deps\\cef\\%{cfg.buildcfg}\\*.dll\" \"%{wks.location}runtime\\%{cfg.buildcfg}\\cef\\\"",
			"copy /y \"%{wks.location}..\\deps\\cef\\%{cfg.buildcfg}\\*.bin\" \"%{wks.location}runtime\\%{cfg.buildcfg}\\cef\\\"",
			"copy /y \"%{wks.location}..\\deps\\cef\\Resources\\*.pak\" \"%{wks.location}runtime\\%{cfg.buildcfg}\\cef\\\"",
			"copy /y \"%{wks.location}..\\deps\\cef\\Resources\\*.dat\" \"%{wks.location}runtime\\%{cfg.buildcfg}\\cef\\\"",
			--"copy /y \"%{wks.location}..\\deps\\cef\\Resources\\locales\\*.pak\" \"%{wks.location}runtime\\%{cfg.buildcfg}\\cef\\locales\\\"",
			"copy /y \"%{wks.location}..\\deps\\cef\\Resources\\locales\\en-US.pak\" \"%{wks.location}runtime\\%{cfg.buildcfg}\\cef\\locales\\\"",
		}

		linkoptions { "-IGNORE:4221", "-IGNORE:4006" }
		removelinks "*"
		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, cef)
