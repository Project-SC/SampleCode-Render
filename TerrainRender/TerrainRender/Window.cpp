#include "Window.h"

using namespace window;

Window::Window(LPCWSTR appName, int height, int width, WNDPROC WndProc, bool isFullscreen)
{
	m_nameApp = appName;
	m_hWindow = height;
	m_wWindow = width;
	m_isFullscreen = isFullscreen;

	m_Instance = GetModuleHandle(NULL);

	// Setup the windows default settings.
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_Instance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_nameApp;
	wc.cbSize = sizeof(WNDCLASSEX);

	if (RegisterClassEx(&wc) == 0) 
	{
		throw Window_Exception("Failed to RegisterClassEx on window init.");
	}

	DEVMODE dmScreenSettings;
	int posX, posY, h, w;

	h = GetSystemMetrics(SM_CYSCREEN);
	w = GetSystemMetrics(SM_CXSCREEN);

	if (m_isFullscreen) 
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsHeight = (unsigned long)h;
		dmScreenSettings.dmPelsWidth = (unsigned long)w;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			throw Window_Exception("ChangeDisplaySettings for fullscreen on window init failed.");
		}
		posX = posY = 0;
		m_hWindow = h;
		m_wWindow = w;
	}
	else
	{
		posX = (w - m_wWindow) / 2;
		posY = (h - m_hWindow) / 2;
		h = m_hWindow;
		w = m_wWindow;
	}

	m_Window = CreateWindowEx(WS_EX_APPWINDOW, m_nameApp, m_nameApp, WS_OVERLAPPEDWINDOW, posX, posY, w, h, NULL, NULL, m_Instance, NULL);
	if (!m_Window) 
	{
		throw Window_Exception("Failed to CreateWindowEx on window init.");
	}

	ShowWindow(m_Window, SW_SHOW);
	SetForegroundWindow(m_Window);

	ShowCursor(false);
}

Window::~Window()
{
	ShowCursor(true);

	if (m_isFullscreen) 
	{
		ChangeDisplaySettings(NULL, 0);
	}

	DestroyWindow(m_Window);
	m_Window = NULL;

	UnregisterClass(m_nameApp, m_Instance);
	m_Instance = NULL;
}
