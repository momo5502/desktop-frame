#include "std_include.hpp"

#include "cef/cef_ui.hpp"
#include "cef/cef_ui_app.hpp"

namespace cef
{
	void cef_ui_app::OnContextInitialized()
	{
		CEF_REQUIRE_UI_THREAD()
	}

	void cef_ui_app::OnBeforeCommandLineProcessing(const CefString& /*process_type*/,
	                                               const CefRefPtr<CefCommandLine> command_line)
	{
		command_line->AppendSwitch("enable-experimental-web-platform-features");
		command_line->AppendSwitch("in-process-gpu");
		command_line->AppendSwitchWithValue("default-encoding", "utf-8");
		command_line->AppendSwitchWithValue("remote-allow-origins", "*");
	}

	void cef_ui_app::OnBeforeChildProcessLaunch(const CefRefPtr<CefCommandLine> command_line)
	{
		command_line->AppendSwitch("cef-subprocess");
	}
}
