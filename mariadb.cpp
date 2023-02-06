#include "stdafx.h"



#ifdef NEED_MDB
HRESULT InstallMDB(MDB_DATA& nd, RUNWWPTR mdb)
{
	if (nd.dir_app.empty() || nd.dir_data.empty() || !mdb.Has())
		return E_POINTER;

	nd.path = nd.dir_app;
	auto confd = [&]()
	{
		std::wstring t3 = nd.data;
		t3 += L"\\my.ini";
		if (!fexists(t3.c_str()))
		{
			std::wstring t = nd.dir_app;
			t += L"\\bin\\";
			t += L"mysql_install_db.exe";
			std::vector<wchar_t> dx(10000);
			SHCreateDirectory(0,nd.data.c_str());
			swprintf_s(dx.data(), 10000, L"\"%s\" -d \"%s\"", t.c_str(), nd.data.c_str());
			Run(dx.data(),true, CREATE_NO_WINDOW, pathfromfile(t.c_str()).c_str());
		}
	};

	std::wstring t = nd.dir_app;
	t += L"\\bin\\";
	t += L"mysqld.exe";

	std::wstring t4 = nd.dir_app;
	t4 += L"\\";
	t4 += L"data";

	if (nd.dir_data.length())
		t4 = nd.dir_data;
	if (fexists(t.c_str()))
	{
		nd.exe = t;
		nd.data = t4;
		confd();
		return S_FALSE;
	}

	HZIP z = 0;
	z = OpenZip((void*)mdb.d, (unsigned int)mdb.sz, 0);
	if (!z)
		return E_FAIL;
	ZIPENTRY ze;
	GetZipItem(z, -1, &ze);
	int numitems = ze.index;
	for (int zi = 0; zi < numitems; zi++)
	{
		ZIPENTRY ze2 = {};
		auto r1 = GetZipItem(z, zi, &ze2); // fetch individual details
		if (r1 != 0)
			return E_FAIL;
		std::wstring fullp = nd.dir_app;
		for (size_t i = 0; i < wcslen(ze2.name); i++)
		{
			if (ze2.name[i] == '/')
				ze2.name[i] = '\\';
		}
		fullp += L"\\";
		auto e2 = ze2.name;
		if (wcsncmp(e2, L"mariadb-", 8) == 0)
		{
			auto e4 = wcschr(e2, '\\');
			if (!e4)
				continue;
			e4++;
			e2 = e4;

		}
		fullp += e2;

		if (ze2.unc_size == 0)
		{
			SHCreateDirectory(0, fullp.c_str());
			continue; //folder
		}
		std::vector<char> ibuf(ze2.unc_size);
		auto r2 = UnzipItem(z, zi, ibuf.data(), ze2.unc_size);
		if (r2 != 0)
			return E_FAIL;

		auto p1 = pathfromfile(fullp.c_str());
		SHCreateDirectory(0, p1.c_str());
		if (!PutFile<>(fullp.c_str(), ibuf, true))
			return E_FAIL;
	}
	CloseZip(z);
	nd.exe = t;
	nd.data = t4;
	confd();
	return S_OK;
}

HANDLE ConfigMDB(MDB_DATA& nd,int MDBPort)
{
	std::vector<wchar_t> dx(10000);
	swprintf_s(dx.data(), 10000, L"\"%s\" -P %i --data \"%s\"", nd.exe.c_str(),MDBPort,nd.data.c_str());
	auto r = Run(dx.data(), false, CREATE_NO_WINDOW, pathfromfile(nd.exe.c_str()).c_str());
	nd.hP = r;
	
	if (1)
	{
		auto t = nd.path;
		t += L"\\bin\\";
		t += L"mysql.exe";
		swprintf_s(dx.data(), 10000, L"\"%s\" -P %i -u root -e \"CREATE DATABASE db1\"", t.c_str(), MDBPort);
		Run(dx.data(), true, CREATE_NO_WINDOW, pathfromfile(t.c_str()).c_str());
	}

	return r;

}
void KillMDB(MDB_DATA& nd)
{
	TerminateProcess(nd.hP, 0);
	CloseHandle(nd.hP);
	nd.hP = INVALID_HANDLE_VALUE;

}

#endif