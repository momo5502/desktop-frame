#pragma once

#include "cef_ui.hpp"
#include "cef_include.h"

namespace cef
{
	class cef_ui_browser
	{
	public:
		cef_ui_browser(cef_ui& ui, const std::string& url);
		~cef_ui_browser();

		cef_ui_browser(cef_ui_browser&&) = delete;
		cef_ui_browser(const cef_ui_browser&) = delete;
		cef_ui_browser& operator=(cef_ui_browser&&) = delete;
		cef_ui_browser& operator=(const cef_ui_browser&) = delete;

		HWND get_window() const;

		void close_browser();
		void reload_browser() const;

	private:
		CefRefPtr<CefBrowser> browser_{};
	};
}