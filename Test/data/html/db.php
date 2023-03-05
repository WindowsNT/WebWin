<?php

$db = new SQLite3("test.db");
function QQZ_SQLite($dbs,$q,$arr = array(),$stmtx = null)
{
    global $lastRowID;
    global $superadmin;

	$stmt = $stmtx;
    if (!$stmt)
        $stmt = $dbs->prepare($q);
    if (!$stmt)
        return null;
    $i = 1;
    foreach($arr as $a)
    {
        $stmt->bindValue($i,$a);
        $i++;
    }
    $a = $stmt->execute();
    $lastRowID = $dbs->lastInsertRowID();
    if ($a === FALSE)
        {
            die("Database busy, please try later.");
        }
    return $a;
}
function QQZ_MySQL(mysqli $dbs,$q,$arr = array())
{
    global $lastRowID;
    if (!is_array($arr)) die("QQZ_MySQL passed not an array.");

	$stmt = $dbs->prepare($q);
    if (!$stmt)
         return null;
    $arx = array();
    $bp = "";
    foreach($arr as $a)
        $bp .= "s";
    if (count($arr) > 0)
    {
        $arx [] = &$bp;
        foreach($arr as &$a)
            $arx [] = &$a;

        call_user_func_array (array($stmt,'bind_param'),$arx);
    }
    $stmt->execute();
    $a = $stmt->get_result();
    $lastRowID = $dbs->insert_id;
    $m = new msql_wrap;
    $m->rx = $a;


    return $m;
}

class msql_wrap
{
    public $rx;
    public function fetchArray()
    {
        if (!$this->rx)
            return null;
        if ($this->rx->num_rows == 0)
            return null;
        return $this->rx->fetch_assoc();
    }
};


// -------------
// Generic SQLite query
function QQ($q,$arr = array(),$stmt = null)
{
	global $db;
    if (!is_array($arr)) die("QQ passed not an array.");
    return QQZ_SQLite($db,$q,$arr,$stmt);
}


 $mysqli = new mysqli("localhost:".MDB_PORT,"root","root","db1");
