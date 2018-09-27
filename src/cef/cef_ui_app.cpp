#include "std_include.hpp"

#include "cef/cef_ui.hpp"
#include "cef/cef_ui_app.hpp"

namespace cef
{
	cef_ui_app::cef_ui_app()
	{

	}

	void cef_ui_app::OnContextInitialized()
	{
		CEF_REQUIRE_UI_THREAD();
	}

	void cef_ui_app::OnBeforeCommandLineProcessing(const CefString& /*process_type*/, CefRefPtr<CefCommandLine> command_line)
	{
		command_line->AppendSwitch("enable-experimental-web-platform-features");
		command_line->AppendSwitch("in-process-gpu");
		command_line->AppendSwitch("enable-media-stream");
		command_line->AppendSwitch("use-fake-ui-for-media-stream");
		command_line->AppendSwitch("enable-speech-input");
		command_line->AppendSwitch("ignore-gpu-blacklist");
		command_line->AppendSwitch("enable-usermedia-screen-capture");
		command_line->AppendSwitchWithValue("default-encoding", "utf-8");

		/*if (IsWindows10OrGreater())
		{
			command_line->AppendSwitch("force-gpu-rasterization");
		}*/

// 		command_line->AppendSwitch("disable-gpu");
// 		command_line->AppendSwitch("disable-gpu-compositing");
// 		command_line->AppendSwitch("enable-begin-frame-scheduling");
	}
}
