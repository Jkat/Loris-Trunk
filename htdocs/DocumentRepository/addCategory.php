<?php
set_include_path(get_include_path().":../../project/libraries:../../php/libraries:");
require_once "NDB_Client.class.inc";
$client =& new NDB_Client();
$client->initialize("../../project/config.xml");

// create Database object
$DB =& Database::singleton();
if(PEAR::isError($DB)) { 
    print "Could not connect to database: ".$DB->getMessage()."<br>\n"; die(); 
}

if ($_POST['category_name'] != '')
	$category_name = $_POST['category_name'];
if ($_POST['parent_id'] != '')
	$parent_id = $_POST['parent_id'];
if ($_POST['comments'] != '')
	$comments = $_POST['comments'];

$DB->insert("document_repository_categories", array("category_name" => $category_name, "parent_id"=>$parent_id,"comments"=>$comments));

?>
