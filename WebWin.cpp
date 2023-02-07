#include "stdafx.h"


bool fexists(const wchar_t* f)
{
	return GetFileAttributes(f) != 0xFFFFFFFF;
}
std::wstring wide(const char* a)
{
	if (!a)
		return L"";
	std::vector<wchar_t> x(strlen(a) + 100);
	MultiByteToWideChar(CP_UTF8, 0, a, (int)strlen(a), x.data(),(int) x.size());
	return x.data();
}
std::string multi(const wchar_t* a)
{
	if (!a)
		return "";
	std::vector<char> x(wcslen(a)*4 + 100);
	WideCharToMultiByte(CP_UTF8, 0, a, (int)wcslen(a), x.data(), (int)x.size(),0,0);
	return x.data();
}

std::wstring pathfromfile(const wchar_t* n)
{
	std::vector<wchar_t> t(1000);
	wcscpy_s(t.data(), 1000, n);
	PathRemoveFileSpec(t.data());
	if (wcslen(t.data()))
	{
		if (t.data()[wcslen(t.data()) - 1] == '\\')
			t.data()[wcslen(t.data()) - 1] = 0;
	}
	return t.data();
}
std::wstring fnamewithoutpath(const wchar_t* n)
{
	auto e = wcsrchr(n, '\\');
	if (!e)
		return n;
	return e + 1;
}


std::wstring WhereSet;
std::wstring GetRootInstallableFolder()
{
	if (WhereSet.length())
		return WhereSet;
	PWSTR p = 0;
	SHGetKnownFolderPath(FOLDERID_Documents, 0, 0, &p);
	if (!p)
		return L"";
	std::wstring j = p;
	CoTaskMemFree(p);
	return j;
}

#ifdef NEED_NGINX
NGINX_DATA nginx_data;
#endif


#ifdef NEED_PHP
PHP_DATA php_data;
#endif


#ifdef NEED_MDB
MDB_DATA mdb_data;
#endif


int RandomPort()
{
	SOCKET a = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in sA = {};
	sA.sin_family = AF_INET;
	auto ia = INADDR_ANY;
	memcpy(&sA.sin_addr, &ia, 4);
	if (bind(a, (sockaddr*)&sA, sizeof(sA)) < 0)
		return 0;
	int slen = sizeof(sA);
	getsockname(a, (sockaddr*)&sA, &slen);
	closesocket(a);
	return ntohs(sA.sin_port);
}

HANDLE Run(const wchar_t* y, bool W, DWORD flg, const wchar_t* fold)
{
	PROCESS_INFORMATION pInfo = { 0 };
	STARTUPINFO sInfo = { 0 };

	std::vector<wchar_t> cd(1000);
	wcscpy_s(cd.data(), 1000,pathfromfile(y).c_str());

	sInfo.cb = sizeof(sInfo);
	wchar_t yy[1000];
	swprintf_s(yy, 1000, L"%s", y);
	if (!CreateProcess(0, yy, 0, 0, 0, flg, 0, fold ? fold : cd.data(), &sInfo, &pInfo))
		return INVALID_HANDLE_VALUE;
	if (W)
		WaitForSingleObject(pInfo.hProcess, INFINITE);
	CloseHandle(pInfo.hThread);
	if (W)
	{
		DWORD ec = 0;
		GetExitCodeProcess(pInfo.hProcess, &ec);
		CloseHandle(pInfo.hProcess);
		return (HANDLE)(size_t)ec;
	}
	else
		return pInfo.hProcess;
}


HWND MainWindow = 0;
unsigned int WM_TASKBARCREATED = RegisterWindowMessage(_T("TaskbarCreated"));
#define MESSAGE_HIDE (WM_USER + 1)
NOTIFYICONDATA nn = { 0 };
HICON hIconMain = 0;

LRESULT CALLBACK WP(HWND hh, UINT mm, WPARAM ww, LPARAM ll)
{
	if (mm == WM_TASKBARCREATED)
		mm = WM_CREATE;


	switch (mm)
	{
		
	case MESSAGE_HIDE:
		{
			if (ll == WM_LBUTTONDOWN || ll == WM_RBUTTONDOWN)
			{
				// Show menu
				POINT p = { 0 };
				GetCursorPos(&p);
				auto hX = CreatePopupMenu();
				AppendMenu(hX, MF_STRING, 199, L"Exit");
				SetForegroundWindow(hh);
				TrackPopupMenu(hX, TPM_CENTERALIGN | TPM_LEFTBUTTON, p.x, p.y, 0, hh, 0);
				DestroyMenu(hX);
				SendMessage(hh, WM_NULL, 0, 0);
			}
			return 0;
		}

	case WM_CREATE:
		{
			nn.cbSize = sizeof(nn);
			nn.hWnd = hh;
			nn.uID = MESSAGE_HIDE;
			nn.uFlags = NIF_ICON | NIF_MESSAGE;
			nn.uCallbackMessage = MESSAGE_HIDE;
			nn.hIcon = hIconMain;
			Shell_NotifyIcon(NIM_ADD, &nn);
			return true;
		}

		 case WM_NOTIFY:
		{
			return 0;
		}

		case WM_COMMAND:
		{
		int lw = LOWORD(ww);
		if (lw == 199)
		{
			Shell_NotifyIcon(NIM_DELETE, &nn);
			DestroyWindow(hh);
		}
			return 0;
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProc(hh, mm, ww, ll);
}

#include "ww.hpp"
extern "C" HRESULT __stdcall RunWW(RUNWW& ww)
{
	hIconMain = ww.hIcon;
	if (ww.WhereAt)
		WhereSet = ww.WhereAt;
#ifdef NEED_NGINX
	nginx_data.dir_app = GetRootInstallableFolder() + L"\\nginx";
	if (ww.DataFolder)
	{
		nginx_data.dir_data = ww.DataFolder;
		nginx_data.dir_data += L"\\nginx";
	}
	else
	{
		nginx_data.dir_data = nginx_data.dir_app;
		nginx_data.dir_data += L"\\html";
	}
	auto e1 = InstallNginx(nginx_data, ww.nginx);
	if (FAILED(e1))
		return 0;
	if (ww.NginxPort == 0)
		ww.NginxPort = RandomPort();
#endif
#ifdef NEED_PHP
	php_data.dir_app = GetRootInstallableFolder() + L"\\php";
	if (ww.DataFolder)
	{
		php_data.dir_data = ww.DataFolder;
		php_data.dir_data += L"\\php";
	}
	else
	{
		php_data.dir_data = php_data.dir_app;
	}
	auto e2 = InstallPHP(php_data, ww.php,ww.phpxdebug);
	if (FAILED(e2))
		return 0;
	if (ww.PHPPort == 0)
		ww.PHPPort = RandomPort();
#endif
#ifdef NEED_MDB
	mdb_data.dir_app = GetRootInstallableFolder() + L"\\mdb";
	if (ww.DataFolder)
	{
		mdb_data.dir_data = ww.DataFolder;
		mdb_data.dir_data += L"\\mdb";
	}
	else
	{
		mdb_data.dir_data = mdb_data.dir_app;
		mdb_data.dir_data += L"\\data";
	}
	auto e3 = InstallMDB( mdb_data, ww.mdb);
	if (FAILED(e3))
		return 0;
	if (ww.MDBPort == 0)
		ww.MDBPort = RandomPort();
#endif

#ifdef NEED_MDB
	auto hM = ConfigMDB(mdb_data,ww.MDBPort);
	if (hM == INVALID_HANDLE_VALUE)
		return 0;
#endif
#ifdef NEED_PHP
	auto hP = ConfigPHP(php_data, ww.PHPPort);
	if (hP == INVALID_HANDLE_VALUE)
		return 0;
#endif
#ifdef NEED_NGINX
	auto hN = ConfigNginx(nginx_data, ww.NginxPort, ww.PHPPort, ww.root.d, ww.root.sz);
	if (hN == INVALID_HANDLE_VALUE)
		return 0;
#endif

	std::vector<wchar_t> x(1000);
#ifdef NEED_NGINX
#ifdef NEED_PHP
	swprintf_s(x.data(), 1000, L"http://localhost:%u/index.php", ww.NginxPort);
#else
	swprintf_s(x.data(), 1000, L"http://localhost:%u", ww.NginxPort);
#endif
	ShellExecute(0, L"open", x.data(), 0, 0, SW_SHOWNORMAL);
#endif

	WNDCLASSEX wC = { 0 };
	wC.cbSize = sizeof(wC);
	wC.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_PARENTDC;
	wC.lpfnWndProc = WP;
	wC.hInstance = GetModuleHandle(0);
	wC.hIcon = hIconMain;
	wC.hCursor = LoadCursor(0, IDC_ARROW);
	wC.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wC.lpszClassName = _T("WW_MAIN");
	wC.hIconSm = hIconMain;
	RegisterClassEx(&wC);

	MainWindow = CreateWindowEx(WS_EX_ACCEPTFILES,
		_T("WW_MAIN"),
		ttitle,
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS |
		WS_CLIPCHILDREN | WS_MAXIMIZE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
		GetModuleHandle(0), 0);

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}



#ifdef NEED_MDB
	KillMDB(mdb_data);
#endif
#ifdef NEED_PHP
	KillPHP(php_data);
#endif
#ifdef NEED_NGINX
	KillNginx(nginx_data);
#endif

	return 0;
}

