#pragma once

#include "cef_ui_handler.hpp"

namespace cef
{
	class cef_ui_lockscreen_handler : public cef_ui_handler
	{
	public:
		using cef_ui_handler::cef_ui_handler;

		~cef_ui_lockscreen_handler() override;

		void fill_window_info(CefWindowInfo& window_info) const override;

		void OnDocumentAvailableInMainFrame(CefRefPtr<CefBrowser> browser) override;

		void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
		bool DoClose(CefRefPtr<CefBrowser> browser) override;

	private:
		std::atomic_bool exit_flag_{};
		std::thread size_watcher_{};

		void stop_watcher();
		void start_watcher(HWND window);
	};
}
