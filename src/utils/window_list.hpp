namespace utils
{
	class window_list
	{
	public:
		window_list();

		std::vector<HWND>::iterator begin();
		std::vector<HWND>::iterator end();

	private:
		std::vector<HWND> windows;

		static int __stdcall enumerator(HWND window, LPARAM param);
	};
}