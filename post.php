<?php
$GLOBAL_iv128 = '1111333355557777';

function add_eventslist_table($table)
{
    $ret = "";
    $sql_users = "SELECT table_name FROM information_schema.tables where table_name='".$table."'";
    $result = mysql_query($sql_users);
    if (!$result) Return "Error SQL1: " . mysql_error();
    if ($row = mysql_fetch_array($result, MYSQL_ASSOC))
    {
	return 0;
    }else{
	$sql_users = "CREATE TABLE ".$table." (id MEDIUMINT NOT NULL AUTO_INCREMENT, username VARCHAR(255) NOT NULL, fromid VARCHAR(255) NOT NULL, toid VARCHAR(255) NOT NULL, eventstatus INT NOT NULL, PRIMARY KEY (id));";
	$result = mysql_query($sql_users);
    	if (!$result) Return "Error SQL2: " . mysql_error();
    }
}

function add_fileslist_table($table)
{
    $ret = "";
    $sql_users = "SELECT table_name FROM information_schema.tables where table_name='".$table."'";
    $result = mysql_query($sql_users);
    if (!$result) Return "Error SQL1: " . mysql_error();
    if ($row = mysql_fetch_array($result, MYSQL_ASSOC))
    {
	return 0;
    }else{
	$sql_users = "CREATE TABLE ".$table." (id MEDIUMINT NOT NULL AUTO_INCREMENT, username VARCHAR(255) NOT NULL, fromid VARCHAR(255) NOT NULL, toid VARCHAR(255) NOT NULL, filename VARCHAR(255) NOT NULL, password VARCHAR(255) NOT NULL, filesize INT NOT NULL, allcount INT NOT NULL, currentcount INT NOT NULL, flag INT NOT NULL, event INT NOT NULL, PRIMARY KEY (id));";
	$result = mysql_query($sql_users);
    	if (!$result) Return "Error SQL2: " . mysql_error();
    }
}

function add_master_table($table)
{
    $ret = "";
    $sql_users = "SELECT table_name FROM information_schema.tables where table_name='".$table."'";
    $result = mysql_query($sql_users);
    if (!$result) Return "Error SQL1: " . mysql_error();
    if ($row = mysql_fetch_array($result, MYSQL_ASSOC))
    {
	return 0;
    }else{
	$sql_users = "CREATE TABLE ".$table." (id MEDIUMINT NOT NULL AUTO_INCREMENT, username CHAR(255) NOT NULL, masterpassword CHAR(255) NOT NULL, filepassword CHAR(255) NOT NULL, PRIMARY KEY (id));";
	$result = mysql_query($sql_users);
    	if (!$result) Return "Error SQL2: " . mysql_error();
    }
}

function add_main_table($table)
{
    $ret = "";
    $sql_users = "SELECT table_name FROM information_schema.tables where table_name='".$table."'";
    $result = mysql_query($sql_users);
    if (!$result) Return "Error SQL1: " . mysql_error();
    if ($row = mysql_fetch_array($result, MYSQL_ASSOC))
    {
	return 0;
    }else{
	$sql_users = "CREATE TABLE ".$table." (id MEDIUMINT NOT NULL AUTO_INCREMENT, username CHAR(255) NOT NULL, clientid CHAR(255) NOT NULL, status MEDIUMINT NOT NULL, PRIMARY KEY (id));";
	$result = mysql_query($sql_users);
    	if (!$result) Return "Error SQL2: " . mysql_error();
    }
}

function add_table($table)
{
    $ret = "";
    $sql_users = "SELECT table_name FROM information_schema.tables where table_name='".$table."'";
    $result = mysql_query($sql_users);
    if (!$result) die('Error: ' . mysql_error());
    if ($row = mysql_fetch_array($result, MYSQL_ASSOC))
    {
	return 0;
    }else{
	$sql_users = "CREATE TABLE ".$table." (id MEDIUMINT NOT NULL AUTO_INCREMENT, clientid VARCHAR(255) NOT NULL, aeskey VARCHAR(255) NOT NULL, aesvector VARCHAR(255) NOT NULL, serverpublic TEXT(65000) NOT NULL, serverprivate TEXT(65000) NOT NULL, clientpublic TEXT(65000) NOT NULL, container TEXT(65000) NOT NULL, datetime DATETIME NOT NULL, status INT NOT NULL, alias VARCHAR(255) NOT NULL, osversion VARCHAR(255) NOT NULL, temp TEXT(65000) NOT NULL, PRIMARY KEY (id));";
	$result = mysql_query($sql_users);
    	if (!$result) die('Error: ' . mysql_error());
    }
}

$err_level = error_reporting(0);  
$link2sql = mysql_connect('localhost', 'cryptone', 'V1de0Games!');
if (!$link2sql)
{
    die('Con error: ' . mysql_error());
}
$db_selected = mysql_select_db('cryptone', $link2sql);
if (!$db_selected) 
{
    die ('dbd select error: ' . mysql_error());
}
error_reporting($err_level); 

function recurseRmdir($dir)
{
  $files = array_diff(scandir($dir), array('.','..'));
  foreach ($files as $file) {
    (is_dir("$dir/$file")) ? recurseRmdir("$dir/$file") : unlink("$dir/$file");
  }
  return rmdir($dir);
}

function isUser( $user )
{
    $sql_users = "SELECT username FROM userlist where username='".bin2hex($user)."'";
    $result = mysql_query($sql_users);
    if (!$result) die('Error: ' . mysql_error());
    if ($row = mysql_fetch_array($result, MYSQL_ASSOC))
    {
	return 1;
    }

	if(!is_dir(getcwd()."/users/")) mkdir(getcwd()."/users/", 0700);
	$usersdir = getcwd()."/users/".bin2hex($user)."_user/";
	if(is_dir($usersdir)) return 1;
	return 0;
}

function uniqidReal($lenght = 32)
{
    // uniqid gives 13 chars, but you could adjust it to your needs.
    if (function_exists("random_bytes")) {
        $bytes = random_bytes(ceil($lenght / 2));
    } elseif (function_exists("openssl_random_pseudo_bytes")) {
        $bytes = openssl_random_pseudo_bytes(ceil($lenght / 2));
    } else {
        throw new Exception("ERROR: no cryptographically secure random function available");
    }
    return substr(bin2hex($bytes), 0, $lenght);
}

function uniqidReal2($lenght = 16)
{
    // uniqid gives 13 chars, but you could adjust it to your needs.
    if (function_exists("random_bytes")) {
        $bytes = random_bytes(ceil($lenght / 2));
    } elseif (function_exists("openssl_random_pseudo_bytes")) {
        $bytes = openssl_random_pseudo_bytes(ceil($lenght / 2));
    } else {
        throw new Exception("ERROR: no cryptographically secure random function available");
    }
    return substr(bin2hex($bytes), 0, $lenght);
}


function MakeSubClientDirSQL( $user, $tmp )
{
	$usersdir = getcwd()."/users/".bin2hex($user)."_user/";
	if (!is_dir($usersdir)) mkdir($usersdir, 0700);
gentmpuseridagain:
	$tmp_userid = uniqidReal();
	if(!strstr(FindUsernameByUserid ( $tmp_userid ),"ERROR:")) goto gentmpuseridagain;
	$tmp_userDir = $usersdir.$tmp_userid."_client".$tmp;
	//if (is_dir($tmp_userDir)) goto gentmpuseridagain;
	mkdir($tmp_userDir, 0700);
return $tmp_userid;
}


function MakeSubClientDir( $user, $tmp )
{
	$usersdir = getcwd()."/users/".bin2hex($user)."_user/";
	if (!is_dir($usersdir)) mkdir($usersdir, 0700);
gentmpuseridagain:
	$tmp_userid = uniqidReal();
	$tmp_userDir = $usersdir.$tmp_userid."_client".$tmp;
	if (is_dir($tmp_userDir)) goto gentmpuseridagain;
	mkdir($tmp_userDir, 0700);
return $tmp_userid;
}

function GetServerSidePrivateKey($tmp_userfile) 
{
 $PubKey = "";
 $flag = FALSE;
 $handle = @fopen($tmp_userfile,"r");
 if ($handle) 
 {
   while (($buffer = fgets($handle, 4096)) !== false) 
   {
     if(strstr($buffer,"-----END PRIVATE KEY-----"))
     {
	$PubKey = $PubKey.$buffer;
	$flag = FALSE;
	fclose($handle);
        return $PubKey;
     }
     if(strstr($buffer,"-----BEGIN PRIVATE KEY-----"))
     {
	$PubKey = $PubKey.$buffer;
	$flag = TRUE;
     }
     if($flag == TRUE)
     {
	$PubKey = $PubKey.$buffer;
     }
   }
   fclose($handle);
 }
}

function GetServerSidePublicKey( $tmp_userfile )
{
 $PubKey = "";
 $flag = FALSE;
 $handle = @fopen($tmp_userfile,"r");
 if ($handle) 
 {
   while (($buffer = fgets($handle, 4096)) !== false) 
   {
     if(strstr($buffer,"-----END PUBLIC KEY-----"))
     {
	$PubKey = $PubKey.$buffer;
	$flag = FALSE;
	fclose($handle);
        return $PubKey;
     }
     if(strstr($buffer,"-----BEGIN PUBLIC KEY-----"))
     {
	$PubKey = $PubKey.$buffer;
	$flag = TRUE;
     }
     if($flag == TRUE)
     {
	$PubKey = $PubKey.$buffer;
     }
   }
   fclose($handle);
 }
}

function MakeKeys( $tmp_userfile )
{
 $config = array(
 "digest_alg" => "sha512",
 "private_key_bits" => 2048,
 "private_key_type" => OPENSSL_KEYTYPE_RSA,
 );
 $openssl_res = openssl_pkey_new($config);
 openssl_pkey_export($openssl_res, $privKey);
 $pubKey = openssl_pkey_get_details($openssl_res);
 $pubKey = $pubKey['key'];
 //echo $privKey;
 $current_content = strlen($privKey)."\n".$privKey.strlen($pubKey)."\n".$pubKey;
 file_put_contents( $tmp_userfile, $current_content );
}

function aes_encryptExt( $key, $iv, $data )
{
 return base64_encode(openssl_encrypt( $data, 'AES-256-CTR', $key, OPENSSL_RAW_DATA, $iv));
}

function aes_decryptExt( $key, $iv, $data )
{
 return openssl_decrypt(base64_decode($data), 'AES-256-CTR', $key, OPENSSL_RAW_DATA, $iv );
}


function aes_encrypt( $key256, $data )
{
 $iv128 = $GLOBALS['GLOBAL_iv128'];
 return base64_encode(openssl_encrypt($data,'AES-256-CTR',$key256,OPENSSL_RAW_DATA,$iv128));
}

function aes_decrypt( $key256, $data )
{
 $iv128 = $GLOBALS['GLOBAL_iv128'];
 $de64data = base64_decode($data);
 $decodedData = openssl_decrypt($de64data,'AES-256-CTR',$key256,OPENSSL_RAW_DATA,$iv128);
 return $decodedData;

}

function UnZipData($data)
{
 $bin = hex2bin($data);
 $unzip = gzuncompress($bin);
return $unzip;
}

function ssl_encrypt2( $source, $key )
{
 $maxlength=117;
 $output='';
 while($source)
 {
  $input= substr($source,0,$maxlength);
  $source=substr($source,$maxlength);
  if(openssl_public_encrypt($input,$encrypted,$key))
  {
echo   $output.=$encrypted;
  }else{
	echo "ERROR: encrypt error";
  }
 }
return $output;
}

function FindTmpUsernameByUserid ( $Userid )
{
	$DataForClient = "ERROR: Error";
	$sql_users = "SELECT username FROM templist WHERE clientid='".$Userid."' AND status=0";
	$result = mysql_query($sql_users);
	if (!$result) {
		die('Error1: ' . mysql_error());
	}
	if($row = mysql_fetch_array($result, MYSQL_ASSOC))
	{
        	$DataForClient = $row["username"];
    	}
	if ($result) mysql_free_result($result);

	return $DataForClient;
}


function FindUsernameByUserid ( $Userid )
{
	$DataForClient = "ERROR: Error";
	$sql_users = "SELECT username FROM userlist WHERE clientid='".$Userid."' AND status=0";
	$result = mysql_query($sql_users);
	if (!$result) die('Error1: ' . mysql_error());
	if($row = mysql_fetch_array($result, MYSQL_ASSOC))
	{
        	$DataForClient = $row["username"];
    	}
	if ($result) mysql_free_result($result);

	return $DataForClient;
}

function FindUsernameByRegUserid ( $Userid )
{
	$DataForClient = "ERROR: Error";
	$sql_users = "SELECT username FROM userlist WHERE clientid='".$Userid."' AND status=1";
	$result = mysql_query($sql_users);
	if (!$result) {
		die('Error1: ' . mysql_error());
	}
	if($row = mysql_fetch_array($result, MYSQL_ASSOC))
	{
        	$DataForClient = $row["username"];
    	}
	if ($result) mysql_free_result($result);

	return $DataForClient;
}

function CheckTimeOnline($lasttime)
{

 date_default_timezone_set("Europe/Moscow");
 $mysqltime = date ("Y-m-d H:i:s");
//echo "Current time: ".$mysqltime."\r\n";
 $p_u = explode(" ", $lasttime);
 $date_u = $p_u[0];
 $time_u = $p_u[1];

 $t_u = explode(":", $time_u);
 $h_u = $t_u[0];
 $m_u = $t_u[1];

 $p_n = explode(" ", $mysqltime);
 $date_n = $p_n[0];
 $time_n = $p_n[1];
 $t_n = explode(":", $time_n);
 $h_n = $t_n[0];
 $m_n = $t_n[1];
 if($date_u == $date_n )
 {
	if($h_u == $h_n )
	{
		$cur_m = (int)$m_n -(int)$m_u;
		if($cur_m <= 3 ) return 1;
	}
 }
return 0;
}


function JobFromClient($data)
{
	$DataForClient = "ERROR: error";
	$config = array(
	"digest_alg" => "sha512",
	"private_key_bits" => 2048,
	"private_key_type" => OPENSSL_KEYTYPE_RSA,
	);
	$answer = "";
	$pieces = explode(":", $data);
	$Userid = $pieces[1];
        $CodedData = $pieces[2];
	$username = FindUsernameByRegUserid ( $Userid );
	if( strstr($username,"Error")) return $DataForClient;
        $client_dir = getcwd()."/users/".$username."_user/".$Userid."_client/";
	$MasterPwdFile = getcwd()."/users/".$username."_user/master.dat";

	$sql_data = "SELECT aeskey, aesvector, container FROM ".$username." WHERE clientid='".mysql_real_escape_string($Userid)."' AND status=1";
	$result = mysql_query($sql_data);
	if (!$result) return "Error SQL1: " . mysql_error();
	if($row = mysql_fetch_array($result, MYSQL_ASSOC))
	{
        	$client_easkey = $row["aeskey"];
        	$client_easvectror = $row["aesvector"];
		$fileData = $row["container"];
    	}else return "ERROR: cliendid not found.";
	$decrypted_data = aes_decryptExt( $client_easkey, $client_easvectror, $CodedData );
	$pieces = explode(":", $decrypted_data);
	$ContainerPassword = $pieces[0];
	$jobData = $pieces[1];
	$job = $pieces[2];
        date_default_timezone_set("Europe/Moscow");
	$decrypted_data = aes_decryptExt( $ContainerPassword, $client_easvectror,  $fileData );
	if(!strstr($decrypted_data,"registered")) return "ERROR job: Container password is incorrect[".$ContainerPassword."]";
	if(strstr($job,"upload"))
	{
	}
	if(strstr($job,"alias"))
	{
		$mysqltime = date ("Y-m-d H:i:s");
		$sql_users1 = "UPDATE ".$username." SET datetime='".$mysqltime."', alias='".mysql_real_escape_string($jobData)."' where clientid='".mysql_real_escape_string($Userid)."'";
		$result = mysql_query($sql_users1);
		if (!$result) return "Error update client data: " . mysql_error();
		$answer = $jobData.":alias";
	}
	if(strstr($job,"ulist"))
	{
		$sql_users = "SELECT clientid, osversion, datetime, alias FROM ".$username." WHERE status=1";
		$result = mysql_query($sql_users);
		if (!$result) return "Error userlist: " . mysql_error();
		$userlist = "";
		$userscount = 0;
		while($row = mysql_fetch_array($result, MYSQL_ASSOC))
		{
        		$u_clientid = $row["clientid"];
			if( strstr($u_clientid,$Userid) ) continue;
			$u_alias = $row["alias"];
			$u_datetime = $row["datetime"];
			$u_osversion = $row["osversion"];
			$userlist = $userlist."ClientID: ".$u_clientid."\r\nAlias: ".$u_alias."\r\nLast login time: ".$u_datetime."\r\nOS Version: ".$u_osversion."\r\n-----\r\n";
			$userscount++;
    		}
		//if ($result) mysql_free_result($result);

		$answer = ":ulist[".$userscount."]\r\n".$userlist;
	}
	if(strstr($job,"olist"))
	{
		$sql_users = "SELECT clientid, osversion, datetime, alias FROM ".$username." WHERE status=1";
		$mysqltime = date ("Y-m-d H:i:s");
	//$pieces = explode(":", $data);
	//$Userid = $pieces[1];
        //$CodedData = $pieces[2];

		$result = mysql_query($sql_users);
		if (!$result) return "Error userlist: " . mysql_error();
		$userlist = "";
		$userscount = 0;
		while($row = mysql_fetch_array($result, MYSQL_ASSOC))
		{
        		$u_clientid = $row["clientid"];
			if( strstr($u_clientid,$Userid) ) continue;
			$u_datetime = $row["datetime"];
			if ( CheckTimeOnline($u_datetime) == 0 ) continue;
			$u_alias = $row["alias"];
			$u_osversion = $row["osversion"];
			$userlist = $userlist."ClientID: ".$u_clientid."\r\nAlias: ".$u_alias."\r\nLast login time: ".$u_datetime."\r\nOS Version: ".$u_osversion."\r\n-----\r\n";
			$userscount++;
    		}
		//if ($result) mysql_free_result($result);

		$answer = ":olist[".$userscount."]\r\n".$userlist;
	}
	if(strstr($job,"keys"))
	{
		$UnpackedData = UnZipData($jobData);
		$openssl_res = openssl_pkey_new($config);
		openssl_pkey_export($openssl_res, $privKey);
		$pubKey = openssl_pkey_get_details($openssl_res);
		$ServerPublicKey = $pubKey['key'];
		$pieces = explode(":", $UnpackedData);
		$newAESKey = $pieces[0];
        	$newAESVector = $pieces[1];
		$newClientPublicKey = $pieces[2];
		$crypted_data = aes_encryptExt( $ContainerPassword, $newAESVector, "registered" );

		$mysqltime = date ("Y-m-d H:i:s");
		$sql_users1 = "UPDATE ".$username." SET datetime='".$mysqltime."', aeskey='".mysql_real_escape_string($newAESKey)."', aesvector='".mysql_real_escape_string($newAESVector)."', serverprivate='".mysql_real_escape_string($privKey)."', clientpublic='".mysql_real_escape_string($newClientPublicKey)."', container='".mysql_real_escape_string($crypted_data)."' where clientid='".mysql_real_escape_string($Userid)."'";
		$result = mysql_query($sql_users1);
		if (!$result) return "Error update client data: " . mysql_error();

		$answer = $ServerPublicKey.":KeysExOK";
	}
	if(strstr($job,"ping"))
	{
		$osVersion = UnZipData($jobData);
		$mysqltime = date ("Y-m-d H:i:s");
		$sql_users1 = "UPDATE ".$username." SET datetime='".$mysqltime."', osversion='".mysql_real_escape_string($osVersion)."' where clientid='".mysql_real_escape_string($Userid)."'";
		$result = mysql_query($sql_users1);
		if (!$result) return "Error update client data: " . mysql_error();

		$Bytes = disk_free_space(getcwd());
		$Type=array("", "kilo", "mega", "giga", "tera", "peta", "exa", "zetta", "yotta");
		$Index=0;
		 while($Bytes>=1024)
		 {
			 $Bytes/=1024;
			 $Index++;
		 }
		$diskfree = "".$Bytes." ".$Type[$Index]."bytes";

		$answer = $diskfree.":pong";
	}

	$crypted_data = aes_encryptExt( $client_easkey, $client_easvectror, $answer );
	$DataForClient = base64_encode(gzcompress($crypted_data, 9));

//file_put_contents("base64.txt", $DataForClient);

	return $DataForClient;
}

function RegisterStageTmp2( $data )
{
	$DataForClient = "ERROR: Error2";
	$config = array(
	"digest_alg" => "sha512",
	"private_key_bits" => 2048,
	"private_key_type" => OPENSSL_KEYTYPE_RSA,
	);
	$pieces = explode(":", $data);
	$Userid = $pieces[1];
        $CodedData = $pieces[2];
	$username = FindTmpUsernameByUserid ( $Userid );
	if( strstr($username,"Error")) return $DataForClient;
        $client_dir = getcwd()."/users/".$username."_user/".$Userid."_clientTMPID/";
	$client_dir_new = getcwd()."/users/".$username."_user/".$Userid."_client/";
	$sql_data = "SELECT aeskey, aesvector FROM ".$username." WHERE clientid='".mysql_real_escape_string($Userid)."' AND status=0";
	$result = mysql_query($sql_data);
	if (!$result) return "Error SQL1: " . mysql_error();
	if($row = mysql_fetch_array($result, MYSQL_ASSOC))
	{
        	$client_easkey = $row["aeskey"];
        	$client_easvectror = $row["aesvector"];
    	}else return "ERROR: cliendid not found.";
	if ($result) mysql_free_result($result);

	$decrypted_data = aes_decryptExt( $client_easkey, $client_easvectror, $CodedData );
	$pieces = explode(":", $decrypted_data);
	$MasterPassword = $pieces[0];
	$ContainerPassword = $pieces[1];
	$AESKeyNew = $pieces[2];
	$AESVectorNew = $pieces[3];
	$KeyPub = $pieces[4];

	$sql_data = "SELECT masterpassword FROM masterlist WHERE username='".$username."'";
	$result = mysql_query($sql_data);
	if (!$result) return "Error SQL1: " . mysql_error();
	if($row = mysql_fetch_array($result, MYSQL_ASSOC))
	{
        	$fileData = $row["masterpassword"];
    	}else return "ERROR: cliendid not found.";
	if ($result) mysql_free_result($result);
//	$iv128 = $GLOBALS['GLOBAL_iv128'];
	$decrypted_data = aes_decryptExt( $MasterPassword, $GLOBALS['GLOBAL_iv128'],  $fileData );
	$sql_users = "delete from templist where clientid='".$Userid."'";
	$result = mysql_query($sql_users);
	if (!$result) return "Error delete userid from tmplist: " . mysql_error();

	if(!strstr($decrypted_data,"master")) 
	{
		recurseRmdir($client_dir);
		$sql_users1 = "DELETE FROM ".$username." WHERE clientid='".mysql_real_escape_string($Userid)."'";
		$result = mysql_query($sql_users1);
		if (!$result) return "Error delete clientid from username table: " . mysql_error();

		return "ERROR: Master password is incorrect";
	}

	$openssl_res = openssl_pkey_new($config);
	openssl_pkey_export($openssl_res, $privKey);
	$pubKey = openssl_pkey_get_details($openssl_res);
	$pubKey = $pubKey['key'];
	$crypted_data2 = aes_encryptExt( $ContainerPassword, $AESVectorNew, "registered" );
//file_put_contents("ContainerPassword", $ContainerPassword);
//file_put_contents("ContainerPassword", $ContainerPassword);

	$answer = $pubKey.":registered";
	$crypted_data = aes_encryptExt( $AESKeyNew, $AESVectorNew, $answer );
	$DataForClient = base64_encode(gzcompress($crypted_data, 9));

	rename($client_dir, $client_dir_new );	

	date_default_timezone_set("Europe/Moscow");
	$mysqltime = date ("Y-m-d H:i:s");
	$sql_users1 = "UPDATE ".$username." SET aeskey='".mysql_real_escape_string($AESKeyNew)."', aesvector='".mysql_real_escape_string($AESVectorNew)."', serverpublic='', serverprivate='".mysql_real_escape_string($privKey)."', clientpublic='".mysql_real_escape_string($KeyPub)."', container='".mysql_real_escape_string($crypted_data2)."', datetime='".$mysqltime."', status=1 where clientid='".mysql_real_escape_string($Userid)."'";
	$result = mysql_query($sql_users1);
	if (!$result) die('Error1: ' . mysql_error());

	$sql_users = "INSERT INTO userlist SET username='".$username."', clientid='".mysql_real_escape_string($Userid)."', status=1";
	$result = mysql_query($sql_users);
	if (!$result) 
	{
		die('Error1: ' . mysql_error());
	}

	return $DataForClient;
}

function RegisterStage2( $data )
{
	$DataForClient = "ERROR: Error";
	$config = array(
	"digest_alg" => "sha512",
	"private_key_bits" => 2048,
	"private_key_type" => OPENSSL_KEYTYPE_RSA,
	);
	$pieces = explode(":", $data);
	$Userid = $pieces[1];
        $CodedData = $pieces[2];
	$username = FindUsernameByUserid ( $Userid );
	if( strstr($username,"Error")) return $DataForClient;
        $client_dir = getcwd()."/users/".$username."_user/".$Userid."_client/";
	$MasterPwdFile = getcwd()."/users/".$username."_user/master.dat";

	$sql_data = "SELECT aeskey, aesvector FROM ".$username." WHERE clientid='".mysql_real_escape_string($Userid)."' AND status=0";
	$result = mysql_query($sql_data);
	if (!$result) return "Error SQL1: " . mysql_error();
	if($row = mysql_fetch_array($result, MYSQL_ASSOC))
	{
        	$client_easkey = $row["aeskey"];
        	$client_easvectror = $row["aesvector"];
    	}else return "ERROR: cliendid not found.";
	if ($result) mysql_free_result($result);

	$decrypted_data = aes_decryptExt( $client_easkey, $client_easvectror, $CodedData );
	$pieces = explode(":", $decrypted_data);
	$MasterPassword = $pieces[0];
	$ContainerPassword = $pieces[1];
	$AESKeyNew = $pieces[2];
	$AESVectorNew = $pieces[3];
	$KeyPub = $pieces[4];

	$crypted_data = aes_encryptExt( $ContainerPassword, $AESVectorNew, "registered" );
	$CryptedMasterData = aes_encryptExt( $MasterPassword, $GLOBALS['GLOBAL_iv128'], "master" );

	$openssl_res = openssl_pkey_new($config);
	openssl_pkey_export($openssl_res, $privKey);
	$pubKey = openssl_pkey_get_details($openssl_res);
	$pubKey = $pubKey['key'];

	date_default_timezone_set("Europe/Moscow");
	$mysqltime = date ("Y-m-d H:i:s");
	$sql_users1 = "UPDATE ".$username." SET aeskey='".mysql_real_escape_string($AESKeyNew)."', aesvector='".mysql_real_escape_string($AESVectorNew)."', serverpublic='', serverprivate='".mysql_real_escape_string($privKey)."', clientpublic='".mysql_real_escape_string($KeyPub)."', container='".mysql_real_escape_string($crypted_data)."', datetime='".$mysqltime."', status=1 where clientid='".mysql_real_escape_string($Userid)."'";
	$result = mysql_query($sql_users1);
	if (!$result) die('Error1: ' . mysql_error());

	$sql_update2 = "UPDATE userlist SET status=1 where clientid='".$Userid."'";
	$result = mysql_query($sql_update2);
	if (!$result) return 'Error update clientid: ' . mysql_error();

	$sql_update3 = "INSERT INTO masterlist SET username='".$username."', masterpassword='".mysql_real_escape_string($CryptedMasterData)."', filepassword=''";
	$result = mysql_query($sql_update3);
	if (!$result) return 'Error update clientid: ' . mysql_error();

	$answer = $pubKey.":registered";
	$crypted_data = aes_encryptExt( $AESKeyNew, $AESVectorNew, $answer );
	$DataForClient = base64_encode(gzcompress($crypted_data, 9));

	return $DataForClient;
}

function RegisterStage1( $data )
{
	$DataForClient = "ERROR: Error";
	$pieces = explode(":", $data);
	$AesKey = $pieces[1];
        $CodedData = $pieces[2];
        $DecodedData = aes_decrypt( $AesKey, $CodedData );
        $RegData = explode(":", $DecodedData);
        $user = $RegData[0];
        $ClientCert = $RegData[1];
	if ( isUser( $user ) == 0 )
	{
		$clientid = MakeSubClientDirSQL( $user, "" );
		add_table(bin2hex($user));
		$usersdir = getcwd()."/users/".bin2hex($user)."_user/";
		$tmp_userid = $clientid;
		$tmp_userDir = $usersdir.$tmp_userid."_client";
		 $config = array(
		 "digest_alg" => "sha512",
		 "private_key_bits" => 2048,
		 "private_key_type" => OPENSSL_KEYTYPE_RSA,
		 );
		 $openssl_res = openssl_pkey_new($config);
		 openssl_pkey_export($openssl_res, $privKey);
		 $pubKey = openssl_pkey_get_details($openssl_res);
		 $PubKey = $pubKey['key'];
		 $PrivKey = $privKey;

		$newAESKey = uniqidReal();
		$newAESVector = uniqidReal2();
		$input = $clientid.":".$newAESKey.":".$newAESVector.":okey";

		if(openssl_public_encrypt($input,$encrypted,$ClientCert))
		{
			$zippedData = gzcompress($encrypted, 9);
			$DataForClient = base64_encode($zippedData);
			
			date_default_timezone_set("Europe/Moscow");
			$mysqltime = date ("Y-m-d H:i:s");
			$sql_users1 = "INSERT INTO ".bin2hex($user)." SET clientid='".mysql_real_escape_string($clientid)."', aeskey='".mysql_real_escape_string($newAESKey)."', aesvector='".mysql_real_escape_string($newAESVector)."', serverpublic='".mysql_real_escape_string($PubKey)."', serverprivate='".mysql_real_escape_string($PrivKey)."', clientpublic='".mysql_real_escape_string($ClientCert)."', container='', datetime='".$mysqltime."', status=0, alias='".mysql_real_escape_string($clientid)."', osversion='', temp=''";
			$result = mysql_query($sql_users1);
			if (!$result) die('Error1: ' . mysql_error());

			$sql_users2 = "INSERT INTO userlist SET username='".bin2hex($user)."', clientid='".mysql_real_escape_string($clientid)."', status=0";
			$result = mysql_query($sql_users2);
			if (!$result)  die('Error1: ' . mysql_error());

			return $DataForClient;

		}
        }else{
		$input = "0:0:0:exists";
		$encrypted = "";
		if(openssl_public_encrypt($input,$encrypted,$ClientCert))
		{
			$zippedData = gzcompress($encrypted, 9);
			$DataForClient = base64_encode($zippedData);
			return $DataForClient;
		}

	}
	return $DataForClient;
}

function RegisterStageTmp1( $data )
{
	$DataForClient = "ERROR: Error1";
	$pieces = explode(":", $data);
	$AesKey = $pieces[1];
        $CodedData = $pieces[2];
        $DecodedData = aes_decrypt( $AesKey, $CodedData );
        $RegData = explode(":", $DecodedData);
        $user = $RegData[0];
        $ClientCert = $RegData[1];
		$clientid = MakeSubClientDir( $user, "TMPID" );
		$usersdir = getcwd()."/users/".bin2hex($user)."_user/";
		$tmp_userid = $clientid;
		$tmp_userDir = $usersdir.$tmp_userid."_clientTMPID";
		 $config = array(
		 "digest_alg" => "sha512",
		 "private_key_bits" => 2048,
		 "private_key_type" => OPENSSL_KEYTYPE_RSA,
		 );
		 $openssl_res = openssl_pkey_new($config);
		 openssl_pkey_export($openssl_res, $privKey);
		 $pubKey = openssl_pkey_get_details($openssl_res);
		 $PubKey = $pubKey['key'];
		 $PrivKey = $privKey;

		$newAESKey = uniqidReal();
		$newAESVector = uniqidReal2();
		$input = $clientid.":".$newAESKey.":".$newAESVector.":okey";

		if(openssl_public_encrypt($input,$encrypted,$ClientCert))
		{
			$zippedData = gzcompress($encrypted, 9);
			$DataForClient = base64_encode($zippedData);

			date_default_timezone_set("Europe/Moscow");
			$mysqltime = date ("Y-m-d H:i:s");
			$sql_users1 = "INSERT INTO ".bin2hex($user)." SET clientid='".mysql_real_escape_string($clientid)."', aeskey='".mysql_real_escape_string($newAESKey)."', aesvector='".mysql_real_escape_string($newAESVector)."', serverpublic='".mysql_real_escape_string($PubKey)."', serverprivate='".mysql_real_escape_string($PrivKey)."', clientpublic='".mysql_real_escape_string($ClientCert)."', container='', datetime='".$mysqltime."', status=0, alias='".mysql_real_escape_string($clientid)."', osversion='', temp=''";
			$result = mysql_query($sql_users1);
			if (!$result) die('Error1: ' . mysql_error());

			$sql_users = "INSERT INTO templist SET username='".bin2hex($user)."', clientid='".mysql_real_escape_string($clientid)."', status=0";
			$result = mysql_query($sql_users);
			if (!$result) 
			{
				die('Error1: ' . mysql_error());
			}

			return $DataForClient;

		}
	return $DataForClient;
}


add_main_table("userlist");
add_main_table("templist");
add_master_table("masterlist");
add_fileslist_table("fileslist");
add_eventslist_table("eventslist");

if(!empty($_POST["var"]))
{
    $var = $_POST["var"];
    $data = UnZipData($var);

    //Create tmp user for add new client to exists USER /case 1
    if(strstr($data,"tm1:"))
    {          
	$state01 = RegisterStageTmp1($data);
	echo $state01;
    }

    //Create tmp user for add new client to exists USER / case 2
    if(strstr($data,"tm2:"))
    {
	$state02 = RegisterStageTmp2($data);
	echo $state02;
    }

    //For new USER first step
    if(strstr($data,"reg:"))
    {
	$state1 = RegisterStage1($data);
	echo $state1;
    }
	//For new USER final step
    if(strstr($data,"fin:"))
    {
	$state2 = RegisterStage2($data);
	echo $state2;
    }
    //Do some job
    if(strstr($data,"job:"))
    {
	$state3 = JobFromClient($data);
	echo $state3;
    }
}
?>