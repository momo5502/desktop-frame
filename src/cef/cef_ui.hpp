#pragma once

#include "cef_include.hpp"

#include "utils/concurrency.hpp"

namespace cef
{
	class cef_ui
	{
	public:
		using command_handler = std::function<void(const rapidjson::Value& request, rapidjson::Document& response)>;
		using command_handlers = std::unordered_map<std::string, command_handler>;

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

		std::optional<int> run_process();

		void add_command(std::string command, command_handler handler);

	private:
		command_handlers command_handlers_;

		using browser_vector = std::vector<CefRefPtr<CefBrowser>>;
		utils::concurrency::container<browser_vector> browsers_{};
	};
}
