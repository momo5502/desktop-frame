#include "std_include.hpp"

#include "utils/nt.hpp"
#include "utils/system_tray.hpp"

namespace utils
{
	namespace
	{
		void create_window_class(const HINSTANCE instance, const HICON icon, const std::string& classname,
		                         const WNDPROC procedure)
		{
			WNDCLASSEXA window_class{};
			ZeroMemory(&window_class, sizeof(window_class));

			window_class.cbSize = sizeof(window_class);
			window_class.hInstance = instance;
			window_class.lpszClassName = classname.data();
			window_class.lpfnWndProc = procedure;
			window_class.style = CS_DBLCLKS;

			window_class.hIcon = icon;
			window_class.hIconSm = icon;
			window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
			window_class.lpszMenuName = nullptr;
			window_class.cbClsExtra = 0;
			window_class.cbWndExtra = 0;
			window_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));

			RegisterClassExA(&window_class);
		}

		std::string generate_classname()
		{
			return "system_tray_" + std::to_string(time(nullptr)) + "_" + std::to_string(rand() | (rand() << 16));
		}

		HWND create_window(const HINSTANCE instance, const std::string& title, const std::string& classname,
		                   void* user_data)
		{
			const auto window = CreateWindowExA(0, classname.data(), title.data(), WS_VISIBLE | WS_POPUP, -100, -100, 1,
			                                    1, HWND_DESKTOP,
			                                    nullptr, instance, nullptr);

			SetWindowLongPtrA(window, GWLP_USERDATA, LONG_PTR(user_data));
			return window;
		}

		NOTIFYICONDATAA create_notify_icon_data(const std::string& title, const HWND window, const HICON icon)
		{
			NOTIFYICONDATAA notify_icon_data{};
			ZeroMemory(&notify_icon_data, sizeof(notify_icon_data));

			notify_icon_data.cbSize = sizeof(notify_icon_data);
			notify_icon_data.hWnd = window;
			notify_icon_data.uID = ID_TRAY_COMMAND;
			notify_icon_data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
			notify_icon_data.uCallbackMessage = WM_SYSTRAY;
			notify_icon_data.hIcon = icon;
			memcpy(notify_icon_data.szTip, title.data(), std::min(title.size() + 1, sizeof(notify_icon_data.szTip) + 1));

			return notify_icon_data;
		}
	}

	system_tray::system_tray(const std::string& title)
	{
		const nt::library main{};
		this->instance_ = main.get_handle();
		this->classname_ = generate_classname();

		const auto icon = LoadIcon(this->instance_, MAKEINTRESOURCE(IDI_ICON_1));

		create_window_class(this->instance_, icon, this->classname_, static_window_procedure);
		this->window_ = create_window(this->instance_, title, this->classname_, this);

		this->menu_ = CreatePopupMenu();
		this->notify_icon_data_ = create_notify_icon_data(title, this->window_, icon);

		ShowWindow(this->window_, SW_SHOW);
		ShowWindow(this->window_, SW_HIDE);
	}

	system_tray::~system_tray()
	{
		Shell_NotifyIconA(NIM_DELETE, &this->notify_icon_data_);
		UnregisterClassA(this->classname_.data(), this->instance_);
	}

	void system_tray::add_item(const std::string& name, std::function<void()> callback)
	{
		this->callbacks_.push_back(std::move(callback));
		AppendMenuA(this->menu_, MF_STRING, ID_TRAY_COMMAND + this->callbacks_.size(), name.data());
	}

	LRESULT system_tray::window_procedure(const HWND hwnd, const UINT message, const WPARAM w_param,
	                                      const LPARAM l_param)
	{
		switch (message)
		{
		case WM_ACTIVATE:
			ShowWindow(hwnd, SW_HIDE);
			Shell_NotifyIconA(NIM_ADD, &this->notify_icon_data_);
			break;

		case WM_NCHITTEST:
			{
				const LRESULT u_hit_test = DefWindowProcA(hwnd, WM_NCHITTEST, w_param, l_param);
				if (u_hit_test == HTCLIENT)
				{
					return HTCAPTION;
				}

				return u_hit_test;
			}

		case WM_COMMAND:
			{
				if ((w_param >> 16) == 0)
				{
					SendMessage(hwnd, WM_NULL, 0, 0);
					this->execute_callback(w_param);
				}

				return 0;
			}

		case WM_CLOSE:
			{
				return 0;
			}

		case WM_DESTROY:
			this->window_ = nullptr;
			PostQuitMessage(0);
			break;

		case WM_SYSTRAY:
			{
				if (w_param == ID_TRAY_COMMAND)
				{
					SetForegroundWindow(hwnd);
				}

				if (l_param == WM_RBUTTONDOWN)
				{
					POINT curPoint;
					GetCursorPos(&curPoint);
					SetForegroundWindow(hwnd);

					TrackPopupMenu(this->menu_, /*TPM_RETURNCMD | TPM_NONOTIFY*/0, curPoint.x, curPoint.y, 0, hwnd,
					               nullptr);
				}
			}
			break;

		case WM_SYSTRAY_END:
			{
				Shell_NotifyIconA(NIM_DELETE, &this->notify_icon_data_);
				PostQuitMessage(0);
			}
			break;

		default:
			break;
		}

		return DefWindowProcA(hwnd, message, w_param, l_param);
	}

	LRESULT CALLBACK system_tray::static_window_procedure(const HWND hwnd, const UINT message, const WPARAM w_param,
	                                                      const LPARAM l_param)
	{
		auto* object = reinterpret_cast<system_tray*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (object)
		{
			return object->window_procedure(hwnd, message, w_param, l_param);
		}

		return DefWindowProcA(hwnd, message, w_param, l_param);
	}

	void system_tray::execute_callback(const size_t id) const
	{
		const size_t index = id - (ID_TRAY_COMMAND + 1);
		if (index < this->callbacks_.size())
		{
			this->callbacks_[index]();
		}
	}
}
