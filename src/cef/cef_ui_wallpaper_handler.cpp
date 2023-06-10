#include "std_include.hpp"

#include "cef/cef_ui_wallpaper_handler.hpp"

namespace cef
{
	cef_ui_wallpaper_handler::~cef_ui_wallpaper_handler()
	{
		this->stop_watcher();
	}

	void cef_ui_wallpaper_handler::OnDocumentAvailableInMainFrame(const CefRefPtr<CefBrowser> browser)
	{
		std::ifstream t("C:/Users/mauri/Desktop/script.js");
		const std::string str((std::istreambuf_iterator<char>(t)),
		                      std::istreambuf_iterator<char>());

		browser->GetMainFrame()->ExecuteJavaScript(str, "", 0);
	}

	void cef_ui_wallpaper_handler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
	{
		cef_ui_handler::OnAfterCreated(browser);

		const auto window = browser->GetHost()->GetWindowHandle();

		this->desktop_.apply(window);
		this->start_watcher(window);
	}

	bool cef_ui_wallpaper_handler::DoClose(CefRefPtr<CefBrowser> browser)
	{
		this->stop_watcher();

		SetParent(browser->GetHost()->GetWindowHandle(), nullptr);
		return cef_ui_handler::DoClose(browser);
	}

	void cef_ui_wallpaper_handler::stop_watcher()
	{
		this->exit_flag_ = true;
		if (this->size_watcher_.joinable())
		{
			this->size_watcher_.join();
			this->size_watcher_ = {};
		}
	}

	void cef_ui_wallpaper_handler::start_watcher(HWND window)
	{
		this->stop_watcher();

		this->exit_flag_ = false;
		this->size_watcher_ = std::thread([window, this]
		{
			while (IsWindow(window) && !this->exit_flag_)
			{
				const auto screen_width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
				const auto screen_height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

				MoveWindow(window, 0, 0, screen_width, screen_height, true);

				std::this_thread::sleep_for(10ms);
			}
		});
	}
}
