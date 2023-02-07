<?php

ini_set('display_errors', 1); error_reporting(E_ALL);

echo 'Hello from WebWin';
require_once "db.php";
QQ("CREATE TABLE IF NOT EXISTS 'A' (ID INTEGER PRIMARY KEY)");

phpinfo();
xdebug_info();

// Start VSCode and run the debugger so the line below breaks
xdebug_break();
echo 'Break xDebug';
