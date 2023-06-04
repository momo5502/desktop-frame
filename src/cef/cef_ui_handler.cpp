#include "std_include.hpp"

#include "cef/cef_ui.hpp"
#include "cef/cef_ui_app.hpp"
#include "cef/cef_ui_handler.hpp"

namespace cef
{
	cef_ui_handler::cef_ui_handler()
	{

	}

	cef_ui_handler::~cef_ui_handler()
	{

	}

	void cef_ui_handler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
	{
		CEF_REQUIRE_UI_THREAD();
		browser_list.push_back(browser);
	}

	void cef_ui_handler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
	{
		CEF_REQUIRE_UI_THREAD();

		SetParent(browser->GetHost()->GetWindowHandle(), nullptr);

		for (auto bit = this->browser_list.begin(); bit != this->browser_list.end(); ++bit)
		{
			if ((*bit)->IsSame(browser))
			{
				browser_list.erase(bit);
				break;
			}
		}

		if (browser_list.empty())
		{
			CefQuitMessageLoop();
		}
	}

	bool cef_ui_handler::DoClose(CefRefPtr<CefBrowser> browser)
	{
		SetParent(browser->GetHost()->GetWindowHandle(), nullptr);
		return false;
	}

	void cef_ui_handler::OnBeforeContextMenu(CefRefPtr<CefBrowser> /*browser*/, CefRefPtr<CefFrame> /*frame*/, CefRefPtr<CefContextMenuParams> /*params*/, CefRefPtr<CefMenuModel> model)
	{
		model->Clear();
	}

	bool cef_ui_handler::OnContextMenuCommand(CefRefPtr<CefBrowser> /*browser*/, CefRefPtr<CefFrame> /*frame*/, CefRefPtr<CefContextMenuParams> /*params*/, int /*command_id*/, CefContextMenuHandler::EventFlags /*event_flags*/)
	{
		return false;
	}

	void cef_ui_handler::OnContextMenuDismissed(CefRefPtr<CefBrowser> /*browser*/, CefRefPtr<CefFrame> /*frame*/)
	{

	}

	bool cef_ui_handler::RunContextMenu(CefRefPtr<CefBrowser> /*browser*/, CefRefPtr<CefFrame> /*frame*/, CefRefPtr<CefContextMenuParams> /*params*/, CefRefPtr<CefMenuModel> /*model*/, CefRefPtr<CefRunContextMenuCallback> /*callback*/)
	{
		return false;
	}

}
