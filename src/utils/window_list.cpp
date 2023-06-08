#include "std_include.hpp"
#include "utils/window_list.hpp"

namespace utils
{
	namespace
	{
		int WINAPI enumerator(const HWND window, const LPARAM param)
		{
			auto& list = *reinterpret_cast<std::vector<HWND>*>(param);
			list.push_back(window);
			return TRUE;
		}
	}

	std::vector<HWND> get_window_list()
	{
		std::vector<HWND> windows{};
		EnumWindows(enumerator, LPARAM(&windows));
		return windows;
	}
}
