#include "std_include.hpp"

#include "frame.hpp"

#include "cef/cef_ui.hpp"

#include "utils/system_tray.hpp"
#include "utils/desktop_window.hpp"

#include <literally/library.hpp>

using namespace literally;

frame::frame()
{
	SetCurrentDirectoryA(dynlib().get_folder().data());
}

int frame::run()
{
	cef::cef_ui cef_ui;
	{
		int result = cef_ui.run_process();
		if (result >= 0) return result;

		//cef_ui.create("https://earth.plus360degrees.com/");
		cef_ui.create("https://www.youtube.com/embed/IWVJq-4zW24?rel=0&autoplay=1&fs=1&modestbranding=1&mute=1&controls=0&showinfo=0&autohide=1&start=1795");
		//cef_ui.create("file:///C:/Users/mauri/Desktop/test.html");
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
