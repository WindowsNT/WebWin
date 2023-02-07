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

* The rc also includes an icon and a ZIP file of the PHP/HTML code. In this solution a tar is called to zip these as a prebuild event.
* Test demonstrates the calling of the library:

```
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

RunWW(w);
```

You can also remove the w.root = {}. In that case the files are already in the "DataFolder" you specify (under a "html" subdirectory). Under "mdb" subdictectory are the MySQL data files.


