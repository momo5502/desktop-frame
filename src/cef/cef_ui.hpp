#pragma once

#pragma warning(push)
#pragma warning(disable: 4100)

#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/cef_base.h"
#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "include/cef_command_line.h"
#include "include/cef_frame.h"
#include "include/cef_web_plugin.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

#pragma warning(pop)

#include <literally/library.hpp>

namespace cef
{
	class cef_ui
	{
	public:
		cef_ui();
		~cef_ui();

		HWND get_window();

		void close_browser();
		void reload_browser();

		int run_process();
		void create(std::string url);
		void work_once();
		void work();

	private:
		bool initialized = false;

		std::string path;
		CefRefPtr<CefBrowser> browser;

		static void invoke_close_browser(CefRefPtr<CefBrowser> browser);
	};
}
