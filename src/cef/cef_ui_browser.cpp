#include "std_include.hpp"

#include "cef/cef_ui_browser.hpp"
#include "cef/cef_ui_handler.hpp"

namespace cef
{
	cef_ui_browser::cef_ui_browser(cef_ui& ui, const std::string& url)
	{
		CefWindowInfo window_info;
		window_info.SetAsPopup(nullptr, "DesktopFrame");
		window_info.bounds.width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		window_info.bounds.height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
		window_info.bounds.x = 0;
		window_info.bounds.y = 0;
		window_info.style = WS_VISIBLE | WS_POPUP;

		const CefBrowserSettings browser_settings{};
		this->browser_ = CefBrowserHost::CreateBrowserSync(window_info, new cef_ui_handler(ui), url,
		                                                   browser_settings, {},
		                                                   {});
		if (!this->browser_)
		{
			throw std::runtime_error("Failed to create CEF browser");
		}
	}

	cef_ui_browser::~cef_ui_browser()
	{
		this->close_browser();
	}

	HWND cef_ui_browser::get_window() const
	{
		if (!this->browser_)
		{
			return nullptr;
		}

		return this->browser_->GetHost()->GetWindowHandle();
	}

	void cef_ui_browser::close_browser()
	{
		auto browser = this->browser_;
		this->browser_ = nullptr;

		if (!browser)
		{
			return;
		}

		CefPostTask(TID_UI, base::BindOnce([](const CefRefPtr<CefBrowser> b)
		{
			if (b)
			{
				b->GetHost()->CloseBrowser(true);
			}
		}, std::move(browser)));
	}

	void cef_ui_browser::reload_browser() const
	{
		if (this->browser_)
		{
			this->browser_->Reload();
		}
	}
}
