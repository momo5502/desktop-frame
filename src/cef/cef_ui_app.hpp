#pragma once

namespace cef
{
	class cef_ui_app : public CefApp, public CefBrowserProcessHandler
	{
	public:
		cef_ui_app();

		virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override
		{
			return this;
		}

		virtual void OnContextInitialized() override;

	protected:
		virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override;


	private:
		IMPLEMENT_REFCOUNTING(cef_ui_app);
	};
}
