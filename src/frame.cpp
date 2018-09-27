#include "std_include.hpp"

#include "frame.hpp"

#include "cef/cef_ui.hpp"

#include "utils/library.hpp"
#include "utils/system_tray.hpp"
#include "utils/desktop_window.hpp"

frame::frame()
{
	SetCurrentDirectoryA(utils::dynlib().get_folder().data());
}

int frame::run()
{
	cef::cef_ui cef_ui;
	{
		int result = cef_ui.run_process();
		if (result >= 0) return result;

		cef_ui.create("http://oos.moxiecode.com/js_webgl/spring/");
	}

	utils::desktop_window desktop;
	{
		desktop.apply(cef_ui.get_window());
	}

	utils::system_tray tray("Desktop Frame");
	{
		tray.add_item("Inspect", []()
		{
			ShellExecuteA(nullptr, "open", "http://localhost:12345", nullptr, nullptr, SW_SHOWNORMAL);
		});

		tray.add_item("Reload", [&cef_ui]()
		{
			cef_ui.reload_browser();
		});

		tray.add_item("Exit", [&cef_ui]()
		{
			cef_ui.close_browser();
		});
	}
	
	cef_ui.work();
	return 0;
}
