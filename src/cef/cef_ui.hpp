#pragma once

#include "cef_include.h"


#include "utils/concurrency.hpp"

namespace cef
{
	class cef_ui
	{
	public:
		cef_ui();
		~cef_ui();

		cef_ui(cef_ui&&) = delete;
		cef_ui(const cef_ui&) = delete;
		cef_ui& operator=(cef_ui&&) = delete;
		cef_ui& operator=(const cef_ui&) = delete;

		void work_once();
		void work();
		void end_work();

		void add_browser(CefRefPtr<CefBrowser> browser);
		void remove_browser(const CefRefPtr<CefBrowser>& browser);
		bool has_browsers() const;

		int run_process();

	private:
		using browser_vector = std::vector<CefRefPtr<CefBrowser>>;
		utils::concurrency::container<browser_vector> browsers_{};
	};
}
