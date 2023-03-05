#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <objbase.h>
#include <atlbase.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wininet.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <map>
#include <functional>
#include <memory>
#include <sstream>
#include <ShlObj.h>
#include <shellapi.h>
#include "unzip.h"

#include "config.h"
#include "ww.hpp"


#ifdef NEED_NGINX
struct NGINX_DATA
{
	std::wstring dir_app;
	std::wstring dir_data;
	std::wstring exe;
	std::wstring conf;
	std::wstring data;
	HANDLE hP = INVALID_HANDLE_VALUE;
};
HRESULT InstallNginx(NGINX_DATA& nd, RUNWWPTR nginx);
HANDLE ConfigNginx(NGINX_DATA& nd,int p1,int p2);
void KillNginx(NGINX_DATA& nd);
#endif


#ifdef NEED_PHP
struct PHP_DATA
{
	std::wstring dir_app;
	std::wstring dir_data;
	std::wstring exe;
	std::wstring cgi;
	HANDLE hP = INVALID_HANDLE_VALUE;
};
HRESULT InstallPHP(PHP_DATA& nd, RUNWWPTR php,RUNWWPTR phpxdebug,int mdbport);
HANDLE ConfigPHP(PHP_DATA& nd, int p1b);
void KillPHP(PHP_DATA& nd);
#endif


#ifdef NEED_MDB
struct MDB_DATA
{
	std::wstring dir_app;
	std::wstring dir_data;
	std::wstring path;
	std::wstring exe;
	std::wstring data;
	HANDLE hP = INVALID_HANDLE_VALUE;
};
HRESULT InstallMDB(MDB_DATA& nd,RUNWWPTR mdb);
HANDLE ConfigMDB(MDB_DATA& nd, int MDBPort);
void KillMDB(MDB_DATA& nd);
#endif

// 
bool fexists(const wchar_t* f);

template <typename T = char>
inline bool LoadFile(const wchar_t* f, std::vector<T>& d)
{
	HANDLE hX = CreateFile(f, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	if (hX == INVALID_HANDLE_VALUE)
		return false;
	LARGE_INTEGER sz = { 0 };
	GetFileSizeEx(hX, &sz);
	d.resize((size_t)(sz.QuadPart / sizeof(T)));
	DWORD A = 0;
	ReadFile(hX, d.data(), (DWORD)sz.QuadPart, &A, 0);
	CloseHandle(hX);
	if (A != sz.QuadPart)
		return false;
	return true;
}

template <typename T = char>
inline bool PutFile(const wchar_t* f, std::vector<T>& d, bool Fw = false)
{
	HANDLE hX = CreateFile(f, GENERIC_WRITE, 0, 0, Fw ? CREATE_ALWAYS : CREATE_NEW, 0, 0);
	if (hX == INVALID_HANDLE_VALUE)
		return false;
	DWORD A = 0;
	WriteFile(hX, d.data(), (DWORD)(d.size() * sizeof(T)), &A, 0);
	CloseHandle(hX);
	if (A != d.size())
		return false;
	return true;
}
std::wstring wide(const char* a);
std::string multi(const wchar_t* a);
std::wstring fnamewithoutpath(const wchar_t* n);
std::wstring pathfromfile(const wchar_t* n);
HANDLE Run(const wchar_t* y, bool W, DWORD flg,const wchar_t* fold = 0);
template <typename T> void RightToLeft(T* ptr, size_t len)
{
	for (size_t i = 0; i < len ; i++)
	{
		if (ptr[i] == '/')
			ptr[i] = '\\';
	}
}

template <typename T> void LeftToRight(T* ptr, size_t len)
{
	for (size_t i = 0; i < len; i++)
	{
		if (ptr[i] == '\\')
			ptr[i] = '/';
	}
}

