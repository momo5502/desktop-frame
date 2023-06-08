#include "std_include.hpp"

#include "cef/cef_ui.hpp"
#include "cef/cef_ui_app.hpp"
#include "cef/cef_ui_handler.hpp"

namespace cef
{
	cef_ui_handler::cef_ui_handler(cef_ui& ui)
		: ui_(ui)
	{
	}

	void cef_ui_handler::OnDocumentAvailableInMainFrame(const CefRefPtr<CefBrowser> browser)
	{
		std::ifstream t("C:/Users/mauri/Desktop/script.js");
		const std::string str((std::istreambuf_iterator<char>(t)),
		                      std::istreambuf_iterator<char>());

		browser->GetMainFrame()->ExecuteJavaScript(str, "", 0);
	}

	void cef_ui_handler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
	{
		CEF_REQUIRE_UI_THREAD();
		this->ui_.add_browser(std::move(browser));
	}

	void cef_ui_handler::OnBeforeClose(const CefRefPtr<CefBrowser> browser)
	{
		CEF_REQUIRE_UI_THREAD();

		SetParent(browser->GetHost()->GetWindowHandle(), nullptr);
		this->ui_.remove_browser(browser);
	}

	bool cef_ui_handler::DoClose(const CefRefPtr<CefBrowser> browser)
	{
		SetParent(browser->GetHost()->GetWindowHandle(), nullptr);
		return false;
	}

	void cef_ui_handler::OnBeforeContextMenu(CefRefPtr<CefBrowser> /*browser*/, CefRefPtr<CefFrame> /*frame*/,
	                                         CefRefPtr<CefContextMenuParams> /*params*/,
	                                         const CefRefPtr<CefMenuModel> model)
	{
		model->Clear();
	}

	bool cef_ui_handler::OnContextMenuCommand(CefRefPtr<CefBrowser> /*browser*/, CefRefPtr<CefFrame> /*frame*/,
	                                          CefRefPtr<CefContextMenuParams> /*params*/, int /*command_id*/,
	                                          EventFlags /*event_flags*/)
	{
		return false;
	}

	void cef_ui_handler::OnContextMenuDismissed(CefRefPtr<CefBrowser> /*browser*/, CefRefPtr<CefFrame> /*frame*/)
	{
	}

	bool cef_ui_handler::RunContextMenu(CefRefPtr<CefBrowser> /*browser*/, CefRefPtr<CefFrame> /*frame*/,
	                                    CefRefPtr<CefContextMenuParams> /*params*/, CefRefPtr<CefMenuModel> /*model*/,
	                                    CefRefPtr<CefRunContextMenuCallback> /*callback*/)
	{
		return false;
	}
}
