#pragma once

#include "cef_ui.hpp"

namespace cef
{
	class cef_ui_handler : public CefClient, public CefDisplayHandler, public CefLifeSpanHandler, public CefLoadHandler,
	                       public CefContextMenuHandler, public CefRequestHandler
	{
	public:
		explicit cef_ui_handler(cef_ui& ui);
		~cef_ui_handler() override = default;

		cef_ui_handler(cef_ui_handler&&) = delete;
		cef_ui_handler(const cef_ui_handler&) = delete;
		cef_ui_handler& operator=(cef_ui_handler&&) = delete;
		cef_ui_handler& operator=(const cef_ui_handler&) = delete;

		CefRefPtr<CefDisplayHandler> GetDisplayHandler() override
		{
			return this;
		}

		CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override
		{
			return this;
		}

		CefRefPtr<CefLoadHandler> GetLoadHandler() override
		{
			return this;
		}

		CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override
		{
			return this;
		}

		CefRefPtr<CefRequestHandler> GetRequestHandler() override
		{
			return this;
		}

		void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
		void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

		void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
		                         CefRefPtr<CefContextMenuParams> params,
		                         CefRefPtr<CefMenuModel> model) override;
		bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
		                          CefRefPtr<CefContextMenuParams> params, int command_id,
		                          EventFlags event_flags) override;
		void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) override;
		bool RunContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
		                    CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model,
		                    CefRefPtr<CefRunContextMenuCallback> callback) override;

	private:
		cef_ui& ui_;

		IMPLEMENT_REFCOUNTING(cef_ui_handler);
	};
}
