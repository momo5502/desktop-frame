#define WM_SYSTRAY WM_USER + 1
#define WM_SYSTRAY_END WM_USER + 2

#define ID_TRAY_COMMAND 1001

namespace utils
{
	class system_tray
	{
	public:
		system_tray(const std::string& title);
		~system_tray();

		void add_item(const std::string& title, std::function<void()> callback);

	private:
		std::string classname_{};

		std::vector<std::function<void()>> callbacks_{};

		HINSTANCE instance_ = nullptr;
		HMENU menu_ = nullptr;
		HWND window_ = nullptr;
		NOTIFYICONDATAA notify_icon_data_{};

		void execute_callback(size_t id) const;

		LRESULT window_procedure(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);
		static LRESULT CALLBACK static_window_procedure(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);
	};
}