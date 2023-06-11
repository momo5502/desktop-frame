#include "std_include.hpp"

#include "utils/nt.hpp"

#include "cef_ui.hpp"
#include "cef_ui_app.hpp"
#include "cef_ui_scheme_handler.hpp"

namespace cef
{
	namespace
	{
		void delay_load_cef()
		{
			static const auto _ = []
			{
				const auto self = utils::nt::library::get_by_address(&delay_load_cef);
				const auto cef_path = self.get_folder() / "cef";

				utils::nt::library::scoped_dll_directory dll_directory{cef_path};

				if (!utils::nt::library::load("libcef.dll"))
				{
					throw std::runtime_error("Failed to load CEF");
				}

				return 0;
			}();
			(void)_;
		}

		CefMainArgs get_cef_main_args()
		{
			const utils::nt::library main{};
			return CefMainArgs(main.get_handle());
		}
	}

	cef_ui::cef_ui()
	{
		delay_load_cef();

		const auto args = get_cef_main_args();

		CefSettings settings;
		settings.no_sandbox = TRUE;
		settings.remote_debugging_port = 12345;

#ifdef DEBUG
		settings.log_severity = LOGSEVERITY_VERBOSE;
#else
		settings.log_severity = LOGSEVERITY_DISABLE;
#endif

		const utils::nt::library main{};
		const auto folder = main.get_folder();

		CefString(&settings.browser_subprocess_path) = main.get_path();
		CefString(&settings.locales_dir_path) = folder / "cef/locales";
		CefString(&settings.resources_dir_path) = folder / "cef";
		CefString(&settings.log_file) = folder / "cef_data/debug.log";
		CefString(&settings.user_data_path) = folder / "cef_data/user";
		CefString(&settings.cache_path) = folder / "cef_data/cache";
		CefString(&settings.locale) = "en-US";

		if (!CefInitialize(args, settings, new cef_ui_app(), nullptr))
		{
			throw std::runtime_error("Failed to initialize CEF");
		}

		CefRegisterSchemeHandlerFactory("http", "momo", new cef_ui_scheme_handler_factory(this->command_handlers_));
	}

	cef_ui::~cef_ui()
	{
		CefShutdown();
	}

	void cef_ui::work_once()
	{
		CefDoMessageLoopWork();
	}

	void cef_ui::work()
	{
		CefRunMessageLoop();
	}

	void cef_ui::end_work()
	{
		CefQuitMessageLoop();
	}

	std::optional<int> cef_ui::run_process()
	{
		const auto args = get_cef_main_args();
		const auto result = CefExecuteProcess(args, nullptr, nullptr);
		if (result >= 0)
		{
			return result;
		}

		return {};
	}

	void cef_ui::add_command(std::string command, command_handler handler)
	{
		this->command_handlers_[std::move(command)] = std::move(handler);
	}

	void cef_ui::add_browser(CefRefPtr<CefBrowser> browser)
	{
		this->browsers_.access([&](browser_vector& browsers)
		{
			for (const auto& b : browsers)
			{
				if (b->IsSame(browser))
				{
					return;
				}
			}

			browsers.push_back(std::move(browser));
		});
	}

	void cef_ui::remove_browser(const CefRefPtr<CefBrowser>& browser)
	{
		const auto end_it =
			this->browsers_.access<bool>([&](browser_vector& browsers)
			{
				for (auto i = browsers.begin(); i != browsers.end(); ++i)
				{
					if ((*i)->IsSame(browser))
					{
						browsers.erase(i);
						break;
					}
				}

				return browsers.empty();
			});

		if (end_it)
		{
			this->end_work();
		}
	}

	bool cef_ui::has_browsers() const
	{
		return this->browsers_.access<bool>([&](const browser_vector& browsers)
		{
			return !browsers.empty();
		});
	}
}
