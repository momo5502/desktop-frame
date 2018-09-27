#include "std_include.hpp"

#include "utils/library.hpp"
#include "utils/system_tray.hpp"

namespace utils
{
	system_tray::system_tray(std::string _title) : title(_title)
	{
		utils::dynlib proc;
		this->instance = proc.get_handle();

		this->menu = CreatePopupMenu();
		this->icon = LoadIcon(this->instance, MAKEINTRESOURCE(IDI_ICON_1));

		this->generate_classname();
		this->initialize_class();
		this->initialize_window();
		this->initialize_notify_icon_data();

		ShowWindow(this->window, SW_SHOW);
		ShowWindow(this->window, SW_HIDE);
	}

	system_tray::~system_tray()
	{
		Shell_NotifyIconA(NIM_DELETE, &this->notify_icon_data);
		UnregisterClassA(this->classname.data(), this->instance);
	}

	void system_tray::add_item(std::string name, std::function<void()> callback)
	{
		this->callbacks.push_back(callback);
		AppendMenuA(this->menu, MF_STRING, ID_TRAY_COMMAND + this->callbacks.size(), name.data());
	}

	void system_tray::generate_classname()
	{
		char buffer[200] = { 0 };
		snprintf(buffer, sizeof(buffer), "%llX_%X", time(nullptr), rand() | (rand() << 16));

		this->classname = "system_tray_";
		this->classname += buffer;
	}

	void system_tray::initialize_class()
	{
		ZeroMemory(&this->window_class, sizeof(this->window_class));

		this->window_class.hInstance = this->instance;
		this->window_class.lpszClassName = this->classname.data();
		this->window_class.lpfnWndProc = system_tray::static_window_procedure;
		this->window_class.style = CS_DBLCLKS;
		this->window_class.cbSize = sizeof(this->window_class);

		this->window_class.hIcon = this->icon;
		this->window_class.hIconSm = this->icon;
		this->window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
		this->window_class.lpszMenuName = NULL;
		this->window_class.cbClsExtra = 0;
		this->window_class.cbWndExtra = 0;
		this->window_class.hbrBackground = HBRUSH(CreateSolidBrush(RGB(255, 255, 255)));

		RegisterClassExA(&this->window_class);
	}

	void system_tray::initialize_notify_icon_data()
	{
		ZeroMemory(&this->notify_icon_data, sizeof(this->notify_icon_data));

		this->notify_icon_data.cbSize = sizeof(this->notify_icon_data);
		this->notify_icon_data.hWnd = this->window;
		this->notify_icon_data.uID = ID_TRAY_COMMAND;
		this->notify_icon_data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		this->notify_icon_data.uCallbackMessage = WM_SYSTRAY;
		this->notify_icon_data.hIcon = this->icon;
		strncpy_s(this->notify_icon_data.szTip, this->title.data(), sizeof(this->notify_icon_data.szTip));
	}

	void system_tray::initialize_window()
	{
		this->window = CreateWindowExA(
			0,
			this->classname.data(),
			this->title.data(),
			WS_VISIBLE | WS_POPUP,
			-100,
			-100,
			1,
			1,
			HWND_DESKTOP,
			NULL,
			this->instance,
			NULL
		);

		SetWindowLongPtrA(this->window, GWLP_USERDATA, LONG_PTR(this));
	}

	LRESULT system_tray::window_procedure(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
	{
		switch (message)
		{
		case WM_ACTIVATE:
			ShowWindow(hwnd, SW_HIDE);
			Shell_NotifyIconA(NIM_ADD, &this->notify_icon_data);
			break;

		case WM_NCHITTEST:
		{
			LRESULT uHitTest = DefWindowProcA(hwnd, WM_NCHITTEST, w_param, l_param);
			if (uHitTest == HTCLIENT)
				return HTCAPTION;
			else
				return uHitTest;
		}

		case WM_COMMAND:
		{
			if ((w_param >> 16) == 0)
			{
				SendMessage(hwnd, WM_NULL, 0, 0);
				this->execute_callback(w_param);
			}
		}

		case WM_CLOSE:
			return 0;

		case WM_DESTROY:
			this->window = nullptr;
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

				TrackPopupMenu(this->menu, /*TPM_RETURNCMD | TPM_NONOTIFY*/0, curPoint.x, curPoint.y, 0, hwnd, NULL);
			}
		}
		break;

		case WM_SYSTRAY_END:
		{
			Shell_NotifyIconA(NIM_DELETE, &this->notify_icon_data);
			PostQuitMessage(0);
		}
		break;
		}

		return DefWindowProcA(hwnd, message, w_param, l_param);
	}

	LRESULT CALLBACK system_tray::static_window_procedure(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
	{
		system_tray* object = reinterpret_cast<system_tray*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (object) return object->window_procedure(hwnd, message, w_param, l_param);
		return DefWindowProcA(hwnd, message, w_param, l_param);
	}

	void system_tray::execute_callback(size_t id)
	{
		size_t index = id - (ID_TRAY_COMMAND + 1);
		if (index >= 0 && index < this->callbacks.size())
		{
			this->callbacks[index]();
		}
	}
}
