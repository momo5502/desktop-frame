#include "std_include.hpp"

#include "cef/cef_ui_popup_handler.hpp"

namespace cef
{
	void cef_ui_popup_handler::fill_window_info(CefWindowInfo& window_info) const
	{
		window_info.bounds.width = 900;
		window_info.bounds.height = 500;
		window_info.style &= ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);
	}
}
