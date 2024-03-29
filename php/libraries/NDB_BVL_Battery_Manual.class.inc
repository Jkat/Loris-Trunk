<?php 
require_once 'NDB_BVL_Battery.class.inc';
require_once 'Database.class.inc';

class NDB_BVL_Battery_Manual extends NDB_BVL_Battery
{

    function addInstrument($testName)
    {
        $DB = Database::singleton();
        // assert that a battery has already been selected
        $error = $this->_assertBatterySelected();
        if($this->isError($error)) return $error;

        // assert that the instrument is valid
        $error = $this->_assertValidInstrument($testName);
        if($this->isError($error)) return $error;

        // get SessionID, UserID
        $query = "SELECT ID as SessionID, UserID FROM session WHERE ID=:SID";
        $rows = $DB->pselect($query, array('SID' => $this->sessionID));
        if($this->isError($rows) || count($rows)==0) {
            return $this->raiseError("Failed to get SessionID, and UserID when trying to add $testName");
        }

        $sessionData = $rows[0];

        // generate a commentID
        $commentID = $this->_createCommentID($testName);
        if($this->isError($commentID)) {
            return $this->raiseError("Unable to generate a valid CommentID when trying to add $testName");
        }

        // insert into the test table
        $success = $DB->insert($testName, array('CommentID'=>$commentID));
        if($this->isError($success)) {
            return $this->raiseError("Unable to add storage record to $commentID $testName");
        }

        $success = $DB->insert($testName, array('CommentID'=>"DDE_" . $commentID));
        if($this->isError($success)) {
            return $this->raiseError("Unable to add storage record to $commentID $testName");
        }

        // insert into the flag table
        $success = $DB->insert('flag', array('SessionID'=>$sessionData['SessionID'],
            'Test_name'=>$testName,
            'CommentID'=>$commentID,
            'UserID'=>$sessionData['UserID'],
            'Testdate'=>null)
        );

        if($this->isError($success)) {
            return $this->raiseError("Unable to add storage record to flag: $testName");
        }    
        $success = $DB->insert('flag', array('SessionID'=>$sessionData['SessionID'],
            'Test_name'=>$testName,
            'CommentID'=>"DDE_" . $commentID,
            'UserID'=>$sessionData['UserID'],
            'Testdate'=>null)
        );

        if($this->isError($success)) {
            return $this->raiseError("Unable to add storage record to flag (DDE): $testName");
        }
    }
}
