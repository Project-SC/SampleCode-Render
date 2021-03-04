#pragma once

#include <Windows.h>
#include <stdexcept>

namespace window 
{
	class Window_Exception : public std::runtime_error 
	{
	public:
		Window_Exception(const char *msg) : std::runtime_error(msg) {}
	};

	class Window
	{
	public:
		Window(LPCWSTR appName, int height, int width, WNDPROC WndProc, bool isFullscreen);
		~Window();

		HWND GetWindow() { return m_Window; }
		int Height() { return m_hWindow; }
		int Width() { return m_wWindow; }

	private:
		HINSTANCE m_Instance;
		HWND m_Window;
		bool m_isFullscreen;
		int m_hWindow;
		int m_wWindow;
		LPCWSTR m_nameApp;
	};
}