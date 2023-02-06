#pragma once
struct RUNWWPTR
{
	const char* d = 0;
	size_t sz = 0;
	bool Has()
	{
		if (d && sz)
			return true;
		return false;
	}
};
struct RUNWW
{
	HICON hIcon = 0;
	const wchar_t* WhereAt = 0;
	const wchar_t* DataFolder = 0;
	RUNWWPTR root;
	RUNWWPTR nginx;
	RUNWWPTR php;
	RUNWWPTR mdb;

	std::wstring SQLData;
	int NginxPort = 0; // random
	int PHPPort = 0; // random
	int MDBPort = 0; // random
};
extern "C" HRESULT __stdcall RunWW(RUNWW & ww);
