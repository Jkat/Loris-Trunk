<?php
require_once 'NDB_Menu_Filter.class.inc';

/**
 * The menu for the reliability checking
 * @package mri
 * @subpackage NIHPD
 */

 
class NDB_Menu_Filter_document_repository extends NDB_Menu_Filter
{

    var $centerIDMap;
    
    function _hasAccess()
    {
        
    	//create user object
    	$user =& User::singleton();
    	if (Utility::isErrorX($user)){
    		return PEAR::raiseError("User error: " . $user->getMessage());
    	}

    	return $user->hasPermission('file_upload');
    	    	
    }

    function _setupVariables()
    {
        $user =& User::singleton();
        if(Utility::isErrorX($user)) {
            return PEAR::raiseError("User Error: ".$user->getMessage());
        }

        // create the centerID map
        $db =& Database::singleton();
        $pscRows = $db->pselect("SELECT CenterID, Name FROM psc", array());
        foreach($pscRows AS $row) {
            $this->centerIDMap[$row['CenterID']] = $row['Name'];
        }
        
        $query = " FROM document_repository v"; 
//        $query .= " WHERE (v.hide_video=false OR v.hide_video IS NULL)";

        // set the class variables
        $this->columns = array('v.record_id', 'v.File_name', 'v.version', 'v.File_size', 'v.File_category', 'v.File_type', 'v.Instrument', 'v.uploaded_by', 'v.For_site', 'v.comments', 'v.Data_dir', 'v.Date_uploaded');

	    $openAccordion = $_GET['openAccordion'];	
	    $this->tpl_data['openAccordion'] = $openAccordion;
	    $filtered = $_GET['filtered'];	
	    $this->tpl_data['filtered'] = $filtered;
        $this->query = $query;
        $this->group_by = '';
        $this->order_by = 'v.File_name';
        $this->headers = array('File_name', 'version', 'File_type', 'Instrument', 'uploaded_by', 'For_site', 'comments', 'Date_uploaded', '', '');
        $this->validFilters = array('v.For_site', 'v.File_name', 'v.File_category', 'v.File_type', 'v.version', 'v.uploaded_by');

        $this->formToFilter = array(
                                    'File_name' => 'v.File_name',
				                    'File_type' => 'v.File_type',
				                    'File_category' => 'v.File_category',
				                    'version' => 'v.version',
                                    'Data_dir'  => 'v.Data_dir',
                                    'For_site' => 'v.For_site',
                                    'uploaded_by' => 'v.uploaded_by',
                                    'Date_uploaded' => 'v.Date_uploaded'
                                    );
        return true;
    }


    function _setFilterForm()
    {
    	// create user object
        $user =& User::singleton();
        if(Utility::isErrorX($user)) {
            return PEAR::raiseError("User Error: ".$user->getMessage());
        }

        $list_of_sites = Utility::getSiteList(true, false);
           // allow to view all sites data through filter
        if ($user->hasPermission('access_all_profiles')) { 
            // get the list of study sites - to be replaced by the Site object
            if(Utility::isErrorX($list_of_sites)) {
                return PEAR::raiseError("DB Error: ".$list_of_sites->getMessage());
            }
            if(is_array($list_of_sites)) $list_of_sites = array(null => 'Any') + $list_of_sites;
        }
        else {
            // allow only to view own site data
            $site =& Site::singleton($user->getData('CenterID'));
            if (Utility::isErrorX($site)) {
                return PEAR::raiseError("DB Error: ".$site->getMessage());
            }
            if ($site->isStudySite()) {
                $list_of_sites = array($user->getData('CenterID') => $user->getData('Site'));
            }
        }
        
        $list_of_visit_labels = array_merge(array(null=>"Any") + Utility::getVisitList());
        $subproject_options = Utility::getSubprojectList();
        $subproject_options = array(null=>'Any') + $subproject_options;

        $DB = Database::singleton();
        $list_of_instruments_q = $DB->pselect("SELECT DISTINCT Instrument FROM document_repository", array());
        $list_of_instruments = array(null => 'Any');
        foreach ($list_of_instruments_q as $instrument) {
            $list_of_instruments[$instrument['Instrument']] = $instrument['Instrument'];
        }

	    $fileNames = $DB->pselect("Select File_name from document_repository", array());
    	$fileTypes = array();
	    $fileT = array();
	    foreach ($fileNames as $name) {
		    $fileExt[$name['File_name']] = $name['File_name'];
        }

        if (is_array($fileExt)) {
	        foreach ($fileExt as $field=>$value) {
		        $parts = pathinfo($value);
		        $extension = $parts['extension'];
		        $DB->update("document_repository", array("File_type"=>$extension), array('File_name'=>$value));
		        $fileT = $DB->pselect("SELECT DISTINCT File_type FROM document_repository order by File_type", array());
	        }
        }


	    foreach($fileT as $fileType) {
		    $fileTypes[$fileType['File_type']] = $fileType['File_type'];
        }

	    $fileTypes = array_merge(array(null=>'Any') + $fileTypes);


        // Form Elements
        $this->addBasicText('File_name', 'File name:'); 
        $this->addBasicText('version', 'Version:'); 
	
	    $fileCategories = array(null=>'Any', 'abstract'=>'Abstract', 'audio_visual'=>'Audio-visual', 'instrument'=>'Instrument', 'manual'=>'Manual', 'minutes'=>'Minutes', 'paper'=>"Paper", 'presentation'=>'Presentation', 'protocol'=>'Protocol', 'spreadsheet_table'=>'Spreadsheets-Table', 'other'=>'Other');

        $this->addSelect('File_type', 'File type:', $fileTypes); 
        $this->addSelect('File_category', 'File category:', $fileCategories); 
	    $this->tpl_data['File_categories'] = $fileCategories;
	    $this->tpl_data['Sites'] = $list_of_sites;
	    $user =& User::singleton();
	    $userID = $user->getData('UserID');
	    $this->tpl_data['User'] = $userID; 
        $this->addSelect('For_site', 'For Site:', $list_of_sites);
        $this->addBasicText('uploaded_by', 'Uploaded By:');
       
        return true;
    }



    /**
     * Sets the template data for the list of candidates, users, etc.
     *
     * @return void
     * @throws PEAR_Error
     * @access private
     */
    function _setDataTable()
    {
        // create instance of config object
    	$config =& NDB_Config::singleton();

	// overriding pagination
	$DB = Database::singleton();
	$perpage = $DB->pselectOne("SELECT count(*) FROM document_repository", array());
    	// configure the page pagination
	    $params = array(
            'mode'       => 'Sliding',
	        'perPage'    => $perpage,
	        'delta'      => 3,
	        'itemData'   => $this->list
	    );
        $pager = Pager::factory($params);
        $this->list = $pager->getPageData();
        $links = $pager->getLinks();
        $this->tpl_data['page_links'] = $links['all'];

        // print out column headings
        $i = 0;
        foreach ($this->headers as $header) {
            $this->tpl_data['headers'][$i]['name'] = $header;
            // format header
            $this->tpl_data['headers'][$i]['displayName'] = ucwords(str_replace('_', ' ', $header));
            // set field ordering
            if(isset($this->filter['order'])) {
                $this->tpl_data['headers'][$i]['fieldOrder'] = ($this->filter['order']['fieldOrder'] == 'ASC') ? 'DESC' : 'ASC';
            }
            $i++;
        }

        // get the template data for the table rows
        if (is_array($this->list) && !empty($this->list)) {
            // print out data rows
            $count = $pager->getOffsetByPageId();
            $success = $this->_setDataTableRows($count[0]);
            if(Utility::isErrorX($success)) {
                return PEAR::raiseError("NDB_Filter::_setDataTable(): ".$success->getMessage());
            }
        }
    }



    function _setDataTableRows($count)
    {
        // This is a hack. It shouldn't be required, and it certainly shouldn't be here.
    	$user =& User::singleton();
        $this->tpl_data['Normal_Perm'] = ($user->hasPermission('file_upload') || $user->hasPermission('data_entry'));
        // end hack

        $site_names = Utility::getSiteList(true, false); 
        // print out
        $x = 0;
        foreach ($this->list as $item) {
            //count column
            $this->tpl_data['items'][$x][0]['value'] = $x + $count;

            //print out data rows
            $i = 1;
            $Data_dir = $item['Data_dir'];
            $record_id = $item['record_id'];
            $File_size = $item['File_size'];
            foreach ($item as $key => $val) {

                // Confused about this line
                if ($key == 'Data_dir') continue;
                if ($key == 'record_id') continue;
                if ($key == 'File_size') continue;

                $this->tpl_data['items'][$x][$i]['name'] = $key;


                if ($key == 'For_site'){
                    $this->tpl_data['items'][$x][$i]['value'] = $site_names[$val];
		        } else {
                    $this->tpl_data['items'][$x][$i]['value'] = $val;
                }

  
                if ($key == 'File_name'){
                   $this->tpl_data['items'][$x][$i]['Data_dir'] = $Data_dir;
                   $this->tpl_data['items'][$x][$i]['File_size'] = $this->format_size($File_size);
                }
                
                if ($key == 'Date_uploaded'){
                   $this->tpl_data['items'][$x][$i+1]['name'] = 'record_id';
                   $this->tpl_data['items'][$x][$i+1]['value'] = $record_id;
       		       $this->tpl_data['items'][$x][$i+2]['name'] = 'record_id_delete';
                   $this->tpl_data['items'][$x][$i+2]['value'] = $record_id;
                }
                
               $i++;
            }
            $x++;
        }
        return true;
    }
    
    function format_size($value){
      if(is_null($value)){
        return null;
      }
      
      if($value >= 1000000000){
        $value = floor($value/1000000000);
        return "{$value} GB";
      }
      if($value >= 1000000){
        $value = floor($value/1000000);
        return "{$value} MB";
      }
      if($value >= 1000){
        $value = floor($value/1000);
        return "{$value} KB";
      }
      
      return "{$value} bytes";
      
    }
}
?>
