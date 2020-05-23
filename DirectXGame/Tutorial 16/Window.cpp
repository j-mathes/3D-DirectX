// Copyright (c) 2019  PardCode.
// All rights reserved.
//
// This file is part of CPP-3D-Game-Tutorial-Series Project, accessible from https://github.com/PardCode/CPP-3D-Game-Tutorial-Series
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License 
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Special thanks to albinopapa https://github.com/albinopapa for contributing to improve this code

#include "Window.h"
#include <stdexcept>
#include <exception>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam); // Declared before constructor

Window::Window()
{	
	//Setting up WNDCLASSEX object
	WNDCLASSEX wc{}; // Using {} to initialize a struct sets all fields to 0 in C++
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = GetModuleHandle(nullptr);
	wc.lpszClassName = "MyWindowClass";
	wc.lpfnWndProc = &WndProc; // needs to be declared before constructor.  Impl can be after

	// If the registration of class fails, the function will return false
	if (!RegisterClassEx(&wc))
		throw std::runtime_error("Failed to register WNDCLASSEX.");

	// When you create a window the dimensions include the non-client area such as the 
	// border and title bar, I'd use the AdjustWindowRectEx API
	RECT rect = { 0, 0, 1024, 768 };
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);

	const auto width = rect.right - rect.left;
	const auto height = rect.bottom - rect.top;

	//Creation the window
	m_hwnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		"MyWindowClass",
		"DirectX Application",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		NULL,
		NULL,
		NULL,
		NULL);

	// If the creation fails return false
	if (!m_hwnd)
		throw std::runtime_error("Filed to initialize Window.");

	// Display the window
	ShowWindow(m_hwnd, SW_SHOW);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) // impl
{
	switch (msg)
	{
	case WM_CREATE:
	{
		// Event fired when the window is created
		// collected here..

		break;
	}

	case WM_SETFOCUS:
	{
		// Event fired when the window get focus
		Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (window) window->onFocus();
		break;
	}
	case WM_KILLFOCUS:
	{
		// Event fired when the window lost focus
		Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		window->onKillFocus();
		break;
	}
	case WM_DESTROY:
	{
		// Event fired when the window is destroyed
		Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		window->onDestroy();
		::PostQuitMessage(0);
		break;
	}

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

void Window::broadcast()
{
	MSG msg{};

	if (!this->m_is_init)
	{
		SetWindowLongPtr(m_hwnd, GWLP_USERDATA,(LONG_PTR)this);
		this->onCreate();

		this->m_is_init = true;
	}

	this->onUpdate();

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Sleep(1);
}

bool Window::isRun()
{
	if (m_is_run)
	{
		broadcast();
	}
	return m_is_run;
}

RECT Window::getClientWindowRect()
{
	RECT rc;
	::GetClientRect(this->m_hwnd, &rc);
	return rc;
}

void Window::onCreate()
{
}

void Window::onUpdate()
{
}

void Window::onDestroy()
{
	m_is_run = false;
}

void Window::onFocus()
{
}

void Window::onKillFocus()
{
}

Window::~Window()
{
	DestroyWindow(m_hwnd);
}