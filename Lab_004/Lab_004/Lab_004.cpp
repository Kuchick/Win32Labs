#include <Windows.h>
#include "resource.h"

LPCWSTR g_szClassName = L"myWindowClass";
const int ID_TIMER = 1;
int bmp[4];
int n = 0;
int n1 = 1;
void DrawBmp(HDC hdc, HBITMAP hBitmap, int xStart, int yStart);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static int cursorX = 0;
	static int cursorY = 0;
	static int speed = 2;
	static int x = 50;
	static int y = 50;
	static bool is_running_by_X = TRUE;
	static bool is_running_by_Y = TRUE;

	switch (msg)
	{
		case WM_CREATE:
		{
			bmp[0] = IDB_BMP1;
			bmp[1] = IDB_BMP2;
			bmp[2] = IDB_BMP3;
			bmp[3] = IDB_BMP4;
			break;
		}
		case WM_COMMAND:
		{
			break;
		}
		case WM_LBUTTONDOWN:
		{
			cursorX = LOWORD(lParam);
			cursorY = HIWORD(lParam);
			is_running_by_X = TRUE;
			is_running_by_Y = TRUE;
			SetTimer(hwnd, 1, 100, NULL);
			break;
		}
		case WM_GETMINMAXINFO:
		{
			MINMAXINFO* mmi = (MINMAXINFO*)lParam;
			mmi->ptMinTrackSize.x = 800;
			mmi->ptMinTrackSize.y = 600;
			mmi->ptMaxTrackSize.x = 800;
			mmi->ptMaxTrackSize.y = 600;
			break;
		}
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			KillTimer(hwnd, ID_TIMER);
			PostQuitMessage(0);
			break;
		case WM_TIMER:
		{
			if (x != cursorX)
			{
				if (x > cursorX)
				{
					if (x - cursorX == 1)
						x -= speed - 1;
					else
						x -= speed;
				}
				if (x < cursorX)
				{
					if (cursorX - x == 1)
						x += speed - 1;
					else
						x += speed;
				}
			}
			else
				is_running_by_X = FALSE;
			if (y != cursorY)
			{
				if (y > cursorY)
				{
					if (y - cursorY == 1)
						y -= speed - 1;
					else
						y -= speed;
				}
				if (y < cursorY)
				{
					if (cursorY - y == 1)
						y += speed - 1;
					else
						y += speed;
				}
			}
			else
				is_running_by_Y = FALSE;
			if (is_running_by_X || is_running_by_Y)
				InvalidateRect(hwnd, NULL, TRUE);
			break;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			HBITMAP bmImage = NULL;
			if (n1 == 0)
			{
				bmImage = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(bmp[0]));
				n1 = 1;
			}
			else
			{
				bmImage = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(bmp[n]));
				n1 = 0;
				n++;
				if (n == 4)
					n = 0;
			}
			if (bmImage == NULL)
				MessageBox(hwnd, L"Can't load bitmap image!", L"Error loading bitmap image.", MB_ICONERROR | MB_OK);
			DrawBmp(hdc, bmImage, x, y);
			EndPaint(hwnd, &ps);
			break;
		}
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Window Registration Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		L"Moving bitmap (Lab_004)",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, L"Window Creation Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}

void DrawBmp(HDC hdc, HBITMAP hBitmap, int xStart, int yStart)
{
	BITMAP bm;
	HDC hdcMem;
	DWORD dwSize;
	POINT ptSize, ptOrg;

	hdcMem = CreateCompatibleDC(hdc);

	SelectObject(hdcMem, hBitmap);
	SetMapMode(hdcMem, GetMapMode(hdc));

	GetObject(hBitmap, sizeof(BITMAP), (LPVOID)&bm);

	ptSize.x = bm.bmWidth;
	ptSize.y = bm.bmHeight;

	DPtoLP(hdc, &ptSize, 1);

	ptOrg.x = 0;
	ptOrg.y = 0;

	DPtoLP(hdcMem, &ptOrg, 1);

	BitBlt(hdc, xStart, yStart, ptSize.x, ptSize.y, hdcMem, ptOrg.x, ptOrg.y, SRCCOPY);

	DeleteDC(hdcMem);
}