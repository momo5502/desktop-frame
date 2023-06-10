#include "std_include.hpp"

#include "cef/cef_ui_browser.hpp"
#include "cef/cef_ui_handler.hpp"

namespace cef
{
	cef_ui_browser::cef_ui_browser(const std::string& title, const std::string& url, const CefRefPtr<cef_ui_handler>& ui_handler)
	{
		CefWindowInfo window_info{};
		window_info.SetAsPopup(nullptr, "");
		ui_handler->fill_window_info(window_info);
		CefString(&window_info.window_name) = title;

		const CefBrowserSettings browser_settings{};
		this->browser_ = CefBrowserHost::CreateBrowserSync(window_info, ui_handler, url,
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

	cef_ui_browser::operator bool() const
	{
		return this->browser_.operator bool();
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
