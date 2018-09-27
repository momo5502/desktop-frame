#define WM_SYSTRAY WM_USER + 1
#define WM_SYSTRAY_END WM_USER + 2

#define ID_TRAY_COMMAND 1001

namespace utils
{
	class system_tray
	{
	public:
		system_tray(std::string title);
		~system_tray();

		void add_item(std::string title, std::function<void()> callback);

	private:
		std::string title;
		std::string classname;

		std::vector<std::function<void()>> callbacks;

		HINSTANCE instance = nullptr;
		HICON icon = nullptr;
		HMENU menu = nullptr;
		HWND window = nullptr;
		WNDCLASSEXA window_class;
		NOTIFYICONDATAA notify_icon_data;

		void generate_classname();
		void initialize_class();
		void initialize_window();
		void initialize_notify_icon_data();

		void execute_callback(size_t id);

		LRESULT window_procedure(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);
		static LRESULT CALLBACK static_window_procedure(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);
	};
}