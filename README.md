# WebWin
A Library to provide a portable NGINX + PHP + MySQL setup for Windows applications

# Instructions

* In config.h define what you want

```
// App Title
inline const wchar_t* ttitle = L"WebWin";

// Latest Nginx
#define NEED_NGINX "1.23.3"

// Latest PHP
#define NEED_PHP "8.2.2"

// Latest MariaDB
#define NEED_MDB "10.10.2"
```

  You always need nginx. If you don't want PHP or MDB, don't define these defines.


* Build WebWin static library
* In Test project, the test.rc contains resources to nginx zip, PHP zip, XDebug zip and MariaDB zip

```	
NGINX DATA "nginx-1.23.3.zip"
PHP DATA "php-8.2.2-nts-Win32-vs16-x64.zip"
MDB DATA "mariadb-10.10.2-winx64.zip"
PHPXDEBUG DATA "php_xdebug-3.2.0-8.2-vs16-x86_64.zip"
```

* Test demonstrates the calling of the library:

```
std::vector<char> nginx;
std::vector<char> php;
std::vector<char> phpxdebug;
std::vector<char> mdb;
ExtractResource(h, L"NGINX", L"DATA", nginx);
ExtractResource(h, L"PHP", L"DATA", php);
ExtractResource(h, L"PHPXDEBUG", L"DATA", phpxdebug);
ExtractResource(h, L"MDB", L"DATA", mdb);

RUNWW w;
w.hIcon = LoadIcon(h, L"ICON_1");
w.nginx = { nginx.data(),nginx.size() };
w.php = { php.data(),php.size() };
w.phpxdebug = { phpxdebug.data(),phpxdebug.size() };
w.mdb = { mdb.data(),mdb.size() };
w.PHPPort = 0;
w.NginxPort = 0;
w.MDBPort = 0;
	
std::vector<wchar_t> cd(1000);
GetCurrentDirectory(1000,cd.data());
std::wstring t = cd.data();
std::wstring t1 = t + L"\\data\\html";
std::wstring t2 = t + L"\\data\\php";
std::wstring t3 = t + L"\\data\\mdb";

w.HtmlFolder = t1.c_str();
w.PHPDataFolder = t2.c_str();
w.MdbFolder = t3.c_str();
w.WhereAt = L"c:\\ww_apps";


RunWW(w);
```





