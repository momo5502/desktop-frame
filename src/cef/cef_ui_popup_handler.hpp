#pragma once

#include "cef_ui_handler.hpp"

namespace cef
{
	class cef_ui_popup_handler : public cef_ui_handler
	{
	public:
		using cef_ui_handler::cef_ui_handler;

		void fill_window_info(CefWindowInfo& window_info) const override;
	};
}
