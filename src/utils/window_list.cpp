#include "std_include.hpp"
#include "utils/window_list.hpp"

namespace utils
{
	window_list::window_list()
	{
		EnumWindows(window_list::enumerator, LPARAM(this));
	}

	std::vector<HWND>::iterator window_list::begin()
	{
		return this->windows.begin();
	}

	std::vector<HWND>::iterator window_list::end()
	{
		return this->windows.end();
	}

	int __stdcall window_list::enumerator(HWND window, LPARAM param)
	{
		window_list* list = reinterpret_cast<window_list*>(param);
		list->windows.push_back(window);
		return TRUE;
	}
}
