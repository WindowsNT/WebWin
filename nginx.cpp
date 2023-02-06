#include "stdafx.h"

#ifdef NEED_NGINX

const char* nginx_conf1 = 
R"(worker_processes  1;
events {
    worker_connections  1024;
}
http {
    include       mime.types;
    default_type  application/octet-stream;
    sendfile        on;
    keepalive_timeout  65;
    server {
        listen       %i;
        server_name  localhost;
        root   %s;
        location / 
		{
            index  index.html index.htm index.php;
        }
		location ~ \.php$ {        
			fastcgi_pass localhost:%i;
	        fastcgi_index index.php;
            fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;        
			include fastcgi_params; 
		}
    }
}
)";


const char* nginx_conf2 =
R"(worker_processes  1;
events {
    worker_connections  1024;
}
http {
    include       mime.types; 
    default_type  application/octet-stream;
    sendfile        on;
    keepalive_timeout  65;
    server {
        listen       %i;
        server_name  localhost;
        root   %s;
        location / {
            index  index.html index.htm;
        }
    }
}
)";


void KillNginx(NGINX_DATA& nd)
{
	std::wstring r = nd.exe;
	r += L" -s quit";
	Run(r.c_str(), true, CREATE_NO_WINDOW,pathfromfile(nd.exe.c_str()).c_str());
	Sleep(1000);
	CloseHandle(nd.hP);
	nd.hP = INVALID_HANDLE_VALUE;

}

HRESULT ExtractNginxData(NGINX_DATA& nd, const char* root_zip_data, size_t rootlen)
{
	SHCreateDirectory(0, nd.data.c_str());
	auto z = OpenZip((void*)root_zip_data, (unsigned int)rootlen, 0);
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
		if (ze2.unc_size == 0)
		{
			continue; //folder
		}
		std::vector<char> ibuf(ze2.unc_size);
		auto r2 = UnzipItem(z, zi, ibuf.data(), ze2.unc_size);
		if (r2 != 0)
			return E_FAIL;

		std::wstring fullp = nd.data;
		RightToLeft(ze2.name, wcslen(ze2.name));
		fullp += L"\\";
		fullp += ze2.name;

		auto p1 = pathfromfile(fullp.c_str());
		SHCreateDirectory(0, p1.c_str());
		if (!PutFile<>(fullp.c_str(), ibuf, true))
			return E_FAIL;
	}

	return S_OK;
}

HANDLE ConfigNginx(NGINX_DATA& nd,int p1,int p2, const char* root_zip_data, size_t rootlen)
{
	std::vector<char> dx(10000);
	auto j = multi(nd.data.c_str());
	LeftToRight(j.data(), j.size());
	if (p2 == 0) // no php
		sprintf_s(dx.data(), 10000, nginx_conf2, p1,j.c_str());
	else
		sprintf_s(dx.data(), 10000, nginx_conf1, p1, j.c_str(), p2);
	
	dx.resize(strlen (dx.data()));
	PutFile<>(nd.conf.c_str(), dx, true);
	if (FAILED(ExtractNginxData(nd,root_zip_data,rootlen)))
		return INVALID_HANDLE_VALUE;
	auto r = Run(nd.exe.c_str(), false, CREATE_NO_WINDOW,pathfromfile(nd.exe.c_str()).c_str());
	nd.hP = r;
	return r;

}

HRESULT InstallNginx(NGINX_DATA& nd, RUNWWPTR nginx)
{
	if (nd.dir_app.empty() || nd.dir_data.empty() || !nginx.Has())
		return E_POINTER;

	std::wstring t = nd.dir_app;
	t += L"\\";
	t += L"nginx.exe";

	std::wstring t2 = nd.dir_app;
	t2 += L"\\";
	t2 += L"conf\\nginx.conf";

	std::wstring t4 = nd.dir_data;

	if (fexists(t.c_str()))
	{
		nd.exe = t;
		nd.conf = t2;
		nd.data = t4; 
		return S_FALSE;
	}

	SHCreateDirectory(0, nd.dir_app.c_str());

	HZIP z = 0;
	z = OpenZip((void*)nginx.d, (unsigned int)nginx.sz, 0);
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
		if (wcsncmp(e2, L"nginx-", 6) == 0)
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
	nd.conf = t2;
	nd.data = t4;
	return S_OK;
}

#endif