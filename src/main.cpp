#include "std_include.hpp"

#include "utils/nt.hpp"
#include "utils/properties.hpp"
#include "utils/system_tray.hpp"

#include "cef/cef_ui.hpp"
#include "cef/cef_ui_browser.hpp"

#include "cef/cef_ui_popup_handler.hpp"
#include "cef/cef_ui_wallpaper_handler.hpp"

namespace
{
	namespace
	{
		std::string get_youtube_url_for_id(const std::string& id)
		{
			return "https://www.youtube.com/embed/" + id +
				"?rel=0&autoplay=1&fs=1&modestbranding=1&mute=1&controls=0&showinfo=0&autohide=1&loop=1";
		}

		std::optional<std::string> load_video_id()
		{
			return utils::properties::load("video");
		}

		void store_video_id(const std::string& video_id)
		{
			utils::properties::store("video", video_id);
		}

		std::string get_video()
		{
			return get_youtube_url_for_id(load_video_id().value_or("l40nk18GUzk"));
		}
	}

	void run(cef::cef_ui& ui)
	{
		cef::cef_ui_browser background{
			"DesktopFrame",
			get_video(),
			new cef::cef_ui_wallpaper_handler(ui)
		};

		ui.add_command("video", [&background](const rapidjson::Value& request, rapidjson::Document& /*response*/)
		{
			if (!request.IsString())
			{
				return;
			}

			const std::string video_id(request.GetString(), request.GetStringLength());
			store_video_id(video_id);

			background.navigate(get_video());
		});

		cef::cef_ui_browser popup{};
		auto open_popup = [&popup, &ui]
		{
			if (!popup)
			{
				popup = cef::cef_ui_browser("DesktopFrame",
				                            "file:///C:/Users/mauri/Desktop/desktop-frame/app.html",
				                            new cef::cef_ui_popup_handler(ui));
			}
		};

		utils::system_tray tray("Desktop Frame");
		{
			tray.add_item("Open Main", open_popup);

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
