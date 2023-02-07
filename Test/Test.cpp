#include <iostream>
#include <vector>
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")

#ifdef _DEBUG
#pragma comment(lib,"..\\x64\\Debug\\webwin.lib")
#else
#pragma comment(lib,"..\\x64\\Release\\webwin.lib")
#endif
#include "..\\ww.hpp"

HRESULT ExtractResource(HINSTANCE hXX, const TCHAR* Name, const TCHAR* ty, std::vector<char>& data)
{
	HRSRC R = FindResource(hXX, Name, ty);
	if (!R)
		return E_NOINTERFACE;
	HGLOBAL hG = LoadResource(hXX, R);
	if (!hG)
		return E_FAIL;
	DWORD S = SizeofResource(hXX, R);
	char* p = (char*)LockResource(hG);
	if (!p)
	{
		FreeResource(R);
		return E_FAIL;
	}
	data.resize(S);
	memcpy(data.data(), p, S);
	FreeResource(R);
	return S_OK;
}

int __stdcall wWinMain(HINSTANCE h, HINSTANCE, LPWSTR, int)
{
	CoInitializeEx(0, COINIT_APARTMENTTHREADED);
	WSADATA wData = {};
	WSAStartup(MAKEWORD(2, 2), &wData);


	std::vector<char> f;
	std::vector<char> nginx;
	std::vector<char> php;
	std::vector<char> phpxdebug;
	std::vector<char> mdb;

	ExtractResource(h, L"FILES", L"DATA", f);
	ExtractResource(h, L"NGINX", L"DATA", nginx);
	ExtractResource(h, L"PHP", L"DATA", php);
	ExtractResource(h, L"PHPXDEBUG", L"DATA", phpxdebug);
	ExtractResource(h, L"MDB", L"DATA", mdb);

	RUNWW w;
	w.hIcon = LoadIcon(h, L"ICON_1");
	w.nginx = { nginx.data(),nginx.size() };
	w.root = { f.data(),f.size() };
	w.php = { php.data(),php.size() };
	w.phpxdebug = { phpxdebug.data(),phpxdebug.size() };
	w.mdb = { mdb.data(),mdb.size() };
	w.PHPPort = 0;
	w.NginxPort = 0;
	w.MDBPort = 51000; // should be something fixed for PHP to connect here
	
	w.DataFolder = L"c:\\ww_data";
	w.WhereAt = L"c:\\ww_apps";

//	w.DataFolder = L"f:\\tools\\WebWin\\Test\\data";
//	w.root = {  };

	RunWW(w);
	return 0;
}
