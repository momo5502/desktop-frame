#include "std_include.hpp"
#include "frame.hpp"

#include "utils/nt.hpp"
#include "utils/system_tray.hpp"
#include "utils/desktop_window.hpp"

#include "cef/cef_ui.hpp"
#include "cef/cef_ui_browser.hpp"


namespace
{
	void spawn_cef_windows(cef::cef_ui& ui)
	{
		utils::desktop_window desktop{};

		cef::cef_ui_browser browser{
			ui,
			"https://www.youtube.com/embed/l40nk18GUzk?rel=0&autoplay=1&fs=1&modestbranding=1&mute=1&controls=0&showinfo=0&autohide=1&loop=1"
		};
		desktop.apply(browser.get_window());


		std::atomic_bool exit{false};
		std::thread watcher([&]
		{
			const auto window = browser.get_window();
			while (IsWindow(window) && !exit)
			{
				MoveWindow(window, 0, 0, GetSystemMetrics(SM_CXVIRTUALSCREEN),
				           GetSystemMetrics(SM_CYVIRTUALSCREEN), true);
				std::this_thread::sleep_for(10ms);
			}
		});

		utils::system_tray tray("Desktop Frame");
		{
			tray.add_item("Inspect", []()
			{
				ShellExecuteA(nullptr, "open", "http://localhost:12345", nullptr, nullptr, SW_SHOWNORMAL);
			});

			tray.add_item("Reload", [&browser]
			{
				browser.reload_browser();
			});

			tray.add_item("Exit", [&browser]
			{
				browser.close_browser();
			});
		}

		ui.work();

		exit = true;
		watcher.join();
	}

	int run()
	{
		cef::cef_ui ui;

		const int result = ui.run_process();
		if (result >= 0)
		{
			return result;
		}

		spawn_cef_windows(ui);
		return 0;
	}
}

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		return run();
	}
	catch (std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "ERROR", MB_ICONERROR);
	}

	return 1;
}
