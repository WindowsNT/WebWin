#include "stdafx.h"

// https://windows.php.net/downloads/releases/php-8.2.2-Win32-vs16-x64.zip


#ifdef NEED_PHP
const char* phpini = R"([PHP]
extension_dir = "ext"
extension=sqlite3
extension=mysqli
extension=bz2
extension=curl
extension=dba
extension=exif
extension=ftp
extension=gd
extension=gmp
extension=imap
extension=mbstring
extension=ldap
extension=openssl
extension=soap
extension=sockets
extension=tidy
extension=zip
extension=pdo_mysql
extension=pdo_sqlite
)";


HRESULT InstallPHP(PHP_DATA& nd, RUNWWPTR php)
{
	if (nd.dir_app.empty() || nd.dir_data.empty() || !php.Has())
		return E_POINTER;


	std::wstring t = nd.dir_app;
	t += L"\\";
	t += L"php.exe";


	std::wstring t2 = nd.dir_app;
	t2 += L"\\";
	t2 += L"php-cgi.exe";

	auto pini = [&]()
	{
		if (1)
		{
			std::wstring phpi = nd.dir_app;
			phpi += L"\\php.ini";
			std::vector<char> x(strlen(phpini));
			memcpy(x.data(), phpini, x.size());
			PutFile<>(phpi.c_str(), x, true);
		}
	};
	

	if (fexists(t.c_str()))
	{
		nd.exe = t;
		nd.cgi = t2;
		pini();
		return S_FALSE;
	}
	


	auto z = OpenZip((void*)php.d, (unsigned int)php.sz, 0);
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
		RightToLeft(ze2.name, wcslen(ze2.name));
		fullp += L"\\";
		auto e2 = ze2.name;
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
	nd.cgi = t2;
	pini();
	return S_OK;
}


void KillPHP(PHP_DATA& nd)
{
	TerminateProcess(nd.hP, 0);
	CloseHandle(nd.hP);
	nd.hP = INVALID_HANDLE_VALUE;
}

HANDLE ConfigPHP(PHP_DATA& nd, int p1)
{
	std::vector<wchar_t> dx(10000);
	swprintf_s(dx.data(), 10000, L"\"%s\" -b 0.0.0.0:%i", nd.cgi.c_str(), p1);
	auto r = Run(dx.data(), false, CREATE_NO_WINDOW,pathfromfile(nd.cgi.c_str()).c_str());
	nd.hP = r;
	return r;

}

#endif