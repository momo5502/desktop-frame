#include "std_include.hpp"

#include "utils/nt.hpp"
#include "utils/system_tray.hpp"

#include "cef/cef_ui.hpp"
#include "cef/cef_ui_browser.hpp"

#include "cef/cef_ui_popup_handler.hpp"
#include "cef/cef_ui_wallpaper_handler.hpp"

namespace
{
	void run(cef::cef_ui& ui)
	{
		cef::cef_ui_browser background{
			"DesktopFrame",
			"https://www.youtube.com/embed/l40nk18GUzk?rel=0&autoplay=1&fs=1&modestbranding=1&mute=1&controls=0&showinfo=0&autohide=1&loop=1",
			new cef::cef_ui_wallpaper_handler(ui)
		};

		cef::cef_ui_browser popup{
			"DesktopFrame",
			"file:///C:/Users/mauri/source/repos/desktop-frame/app.html",
			new cef::cef_ui_popup_handler(ui)
		};

		utils::system_tray tray("Desktop Frame");
		{
			tray.add_item("Inspect", []
			{
				ShellExecuteA(nullptr, "open", "http://localhost:12345", nullptr, nullptr, SW_SHOWNORMAL);
			});

			tray.add_item("Reload", [&background]
			{
				background.reload_browser();
			});

			tray.add_item("Exit", [&background, &popup]
			{
				popup.close_browser();
				background.close_browser();
			});
		}

		ui.work();
	}

	int main()
	{
		srand(static_cast<uint32_t>(time(nullptr) ^ ~(GetTickCount64() * GetCurrentProcessId())));

		cef::cef_ui ui;

		const auto result = ui.run_process();
		if (result)
		{
			return *result;
		}

		run(ui);
		return 0;
	}
}

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		return main();
	}
	catch (std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "ERROR", MB_ICONERROR);
	}

	return 1;
}
