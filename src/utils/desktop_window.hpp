namespace utils
{
	class desktop_window
	{
	public:
		desktop_window();
		~desktop_window();

		void apply(HWND window);

	private:
		HWND worker_ = nullptr;

		void spawn_worker();
		void locate_worker();

		void hide();
	};
}