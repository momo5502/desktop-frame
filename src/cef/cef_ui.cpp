#include "std_include.hpp"

#include "cef/cef_ui.hpp"
#include "cef/cef_ui_app.hpp"
#include "cef/cef_ui_handler.hpp"

using namespace literally;

namespace cef
{
	void cef_ui::work_once()
	{
		CefDoMessageLoopWork();
	}

	void cef_ui::work()
	{
		CefRunMessageLoop();
	}

	int cef_ui::run_process()
	{
		dynlib proc;
		CefMainArgs args(proc.get_handle());

		return CefExecuteProcess(args, nullptr, nullptr);
	}

	void cef_ui::create(std::string url)
	{
		if (this->browser) return;

		dynlib proc;
		CefMainArgs args(proc.get_handle());

		CefSettings settings;
		settings.no_sandbox = TRUE;
		//settings.single_process = TRUE;
		//settings.windowless_rendering_enabled = TRUE;
		//settings.pack_loading_disabled = FALSE;
		settings.remote_debugging_port = 12345;

#ifdef DEBUG
		settings.log_severity = LOGSEVERITY_VERBOSE;
#else
		settings.log_severity = LOGSEVERITY_DISABLE;
#endif

		CefString(&settings.browser_subprocess_path) = proc.get_path();
		CefString(&settings.locales_dir_path) = this->path + "cef\\locales";
		CefString(&settings.resources_dir_path) = this->path + "cef";
		CefString(&settings.log_file) = this->path + "cef_data\\debug.log";
		CefString(&settings.user_data_path) = this->path + "cef_data\\user";
		CefString(&settings.cache_path) = this->path + "cef_data\\cache";
		CefString(&settings.locale) = "en-US";

		this->initialized = CefInitialize(args, settings, new cef_ui_app(), 0);

		CefBrowserSettings browser_settings;
		//browser_settings.windowless_frame_rate = 60;
		browser_settings.web_security = STATE_DISABLED;

		CefWindowInfo window_info;
		window_info.width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		window_info.height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
		window_info.x = 0;
		window_info.y = 0;
		window_info.style = WS_VISIBLE | WS_POPUP;

		this->browser = CefBrowserHost::CreateBrowserSync(window_info, new cef_ui_handler(), url, browser_settings, NULL);
	}

	HWND cef_ui::get_window()
	{
		if (!this->browser) return nullptr;
		return this->browser->GetHost()->GetWindowHandle();
	}

	void cef_ui::invoke_close_browser(CefRefPtr<CefBrowser> browser)
	{
		if (!browser) return;
		browser->GetHost()->CloseBrowser(true);
	}

	void cef_ui::close_browser()
	{
		if (!this->browser) return;
		CefPostTask(TID_UI, base::Bind(&cef_ui::invoke_close_browser, this->browser));
		this->browser = nullptr;
	}

	void cef_ui::reload_browser()
	{
		if (!this->browser) return;
		this->browser->Reload();
	}

	cef_ui::cef_ui()
	{
		dynlib self = dynlib::get_by_address(dynlib::get_by_address);
		this->path = self.get_folder();

		dynlib::add_load_path(this->path + "cef");
		dynlib libcef("libcef.dll", true);
		if (!libcef.is_valid() || !libcef.delay_import())
		{
			throw std::runtime_error("Unable to import libcef");
		}

		CefEnableHighDPISupport();
	}

	cef_ui::~cef_ui()
	{
		if (this->browser)
		{
			this->close_browser();
			this->work();
		}

		if (this->initialized)
		{
			CefShutdown();
		}
	}
}
