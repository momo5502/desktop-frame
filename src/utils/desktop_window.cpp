#include "std_include.hpp"
#include "window_list.hpp"
#include "utils/desktop_window.hpp"

namespace utils
{
	desktop_window::desktop_window()
	{
		this->spawn_worker();
		this->locate_worker();

		if (!this->worker)
		{
			throw std::runtime_error("Unable to locate worker");
		}
	}

	desktop_window::~desktop_window()
	{
		this->hide();
	}

	void desktop_window::apply(HWND window)
	{
		SetParent(window, this->worker);
		ShowWindow(this->worker, SW_SHOWNORMAL);
		UpdateWindow(this->worker);
	}

	void desktop_window::spawn_worker()
	{
		DWORD_PTR result;
		HWND window = FindWindowA("Progman", nullptr);
		SendMessageTimeoutA(window, 0x052c, 0, 0, SMTO_BLOCK, 1000, &result);
	}

	void desktop_window::locate_worker()
	{
		window_list windows;

		for (auto window : windows)
		{
			HWND def_view = FindWindowExA(window, nullptr, "SHELLDLL_DefView", nullptr);
			if (def_view)
			{
				this->worker = FindWindowExA(nullptr, window, "WorkerW", nullptr);
				this->hide();
				return;
			}
		}
	}

	void desktop_window::hide()
	{
		if (!this->worker) return;
		ShowWindow(this->worker, SW_HIDE);
		UpdateWindow(this->worker);
	}
}
