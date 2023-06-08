#pragma once

namespace cef
{
	class cef_ui_handler : public CefClient, public CefDisplayHandler, public CefLifeSpanHandler, public CefLoadHandler,
	                       public CefContextMenuHandler, CefRequestHandler
	{
	public:
		explicit cef_ui_handler();
		~cef_ui_handler();

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

		void OnDocumentAvailableInMainFrame(CefRefPtr<CefBrowser> browser) override;

		//virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;

		virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
		virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

		virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;

		virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
		                                 CefRefPtr<CefContextMenuParams> params,
		                                 CefRefPtr<CefMenuModel> model) override;
		virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
		                                  CefRefPtr<CefContextMenuParams> params, int command_id,
		                                  CefContextMenuHandler::EventFlags event_flags) override;
		virtual void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) override;
		virtual bool RunContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
		                            CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model,
		                            CefRefPtr<CefRunContextMenuCallback> callback) override;

	private:
		std::vector<CefRefPtr<CefBrowser>> browser_list;

		IMPLEMENT_REFCOUNTING(cef_ui_handler);
	};
}
