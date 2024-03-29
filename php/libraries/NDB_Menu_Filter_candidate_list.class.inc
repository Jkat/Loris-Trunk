<?php
require_once 'NDB_Menu_Filter.class.inc';
require_once 'NDB_Form_participant_status.class.inc';
/**
* The candidate list menu
 * @package main
*/
class NDB_Menu_Filter_candidate_list extends NDB_Menu_Filter
{
    /**
    * overloading this method to allow access to site users (their own site only) and users w/ multisite privs
    * @note   overloaded function
    * @return bool
    * @access private
    */
    function _hasAccess()
    {
        // create user object
        $user =& User::singleton();
        if(Utility::isErrorX($user)) {
            return PEAR::raiseError("User Error: ".$user->getMessage());
        }

        // allow only to view own site data
        $site =& Site::singleton($user->getData('CenterID'));
        if (Utility::isErrorX($site)) {
            return PEAR::raiseError("DB Error: ".$site->getMessage());
        }
        
        return ($user->hasPermission('access_all_profiles') ||  $site->isStudySite());
    }

    function _setupVariables()
    {
        $this->tpl_data['toggled_visible']=false;
        // set the class variables
        $this->columns = array('psc.Name AS PSC', 'candidate.CandID AS DCCID','participant_status_options.ID as Participant_Status', 'candidate.PSCID', 'candidate.Gender', 'min(candidate.ProjectID) as Project', 'min(session.SubprojectID) as Subproject', 'DATE_FORMAT(candidate.DoB,\'%Y-%m-%d\') AS DoB');
        $config=&NDB_Config::singleton();
        $useProjects = $config->getSetting("useProjects");
        // set the class variables
        $this->columns = array('psc.Name AS Site',
            'candidate.CandID AS DCCID',
            'candidate.PSCID',
            'candidate.Gender',
            'participant_status_options.ID AS Participant_Status',
        );
        if ($useProjects === "true") {
            $this->columns[] = 'min(candidate.ProjectID) as Project';
        }
        $this->columns=array_merge($this->columns,
            array('min(session.SubprojectID) as Subproject',
            'DATE_FORMAT(candidate.DoB,\'%Y-%m-%d\') AS DoB',
            'session.Scan_done as scan_Done')
        );
        if($config->getSetting("useEDC")==="true") {
        	$this->columns[]='DATE_FORMAT((candidate.EDC),\'%Y-%m-%d\') AS EDC';
        	$this->formToFilter['edc'] = 'candidate.EDC';
        	$this->validFilters[]='candidate.EDC';
        }

        $this->columns=array_merge($this->columns,
            array('count(DISTINCT session.Visit_label) as Visit_count',
            'max(session.Current_stage) as Latest_visit_status',
            'min(feedback_bvl_thread.Status+0) as Feedback')
        );

        
        $this->query = " FROM psc, candidate LEFT JOIN session ON candidate.CandID = session.CandID AND session.Active = 'Y'
         LEFT JOIN feedback_bvl_thread ON (candidate.CandID=feedback_bvl_thread.CandID)
         LEFT JOIN participant_status ON (participant_status.CandID=candidate.CandID)
         LEFT JOIN participant_status_options ON (participant_status.participant_status=participant_status_options.ID)
        WHERE candidate.CenterID = psc.CenterID AND candidate.Entity_type = 'Human' AND candidate.Active = 'Y'";
        $user =& User::singleton();
        if (!$user->hasPermission('access_all_profiles')) {
            $this->query .= " AND candidate.CenterID=" . $user->getCenterID();
        }
        
        $this->group_by = 'candidate.CandID';
        $this->order_by = 'psc.Name, candidate.CandID DESC, session.VisitNo';
        $this->validFilters = array('participant_status_options.ID','candidate.CenterID', 'candidate.CandID', 'candidate.PSCID', 'candidate.Gender', 'candidate.ProjectID', 'session.SubprojectID', 'candidate.DoB', 'count(DISTINCT session.Visit_label)', 'max(session.Current_stage)', 'coalesce(min(feedback_bvl_thread.Status+1), 1)','session.Scan_done');
        $this->validHavingFilters = array('count(DISTINCT session.Visit_label)','max(session.Current_stage)', 'coalesce(min(feedback_bvl_thread.Status+1), 1)');
		
        $this->formToFilter = array(
                                    'centerID' => 'candidate.CenterID',
                                    'DCCID' => 'candidate.CandID',
                                    'PSCID' => 'candidate.PSCID',
                                    'gender' => 'candidate.Gender',
                                    'SubprojectID' => 'session.SubprojectID',
                                    'ProjectID' => 'candidate.ProjectID',
                                    'dob' => 'candidate.DoB',
                                    'Visit_Count' => 'count(DISTINCT session.Visit_label)', // Need distinct because of joining with feedback_bvl_thread
                                    'Latest_Visit_Status' => 'max(session.Current_stage)',
                                    'Feedback' => 'coalesce(min(feedback_bvl_thread.Status+1), 1)',
                                    'Participant_Status' => 'participant_status_options.ID',
                                    'scan_done' => 'session.Scan_done'
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

        // PSC
        if ($user->hasPermission('access_all_profiles')) {
            // get the list of study sites - to be replaced by the Site object
            $list_of_sites =& Utility::getSiteList();
            if(Utility::isErrorX($list_of_sites)) {
                return PEAR::raiseError("DB Error: ".$list_of_sites->getMessage());
            }
            if(is_array($list_of_sites)) $list_of_sites = array('' => 'All') + $list_of_sites;
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

        // SubprojectID
        $list_of_subprojects =& Utility::getSubprojectList();
        $list_of_projects =& Utility::getProjectList();

        // list of feedback statuses
        $feedback_status_options = array(null => 'All','1' => 'None', '2'=>'opened','3'=>'answered','4'=>'closed','5'=>'comment');
        $latest_visit_status_options = array(null => 'All', 'Not Started' => 'Not Started', 'Screening' => 'Screening', 'Visit' => 'Visit', 'Approval' => 'Approval',
                                            /* 'Subject' => 'Subject', */'Recycling Bin' => 'Recycling Bin');
    
        $participant_status_options=   array(null=>'All') +  NDB_Form_participant_status::getParticipantStatusOptions();
        $scan_options = array(null=>'All','Y'=>'Yes','N'=>'No');
        // add form elements
        $this->addSelect('centerID', 'Site:', $list_of_sites);
        $this->addBasicText('DCCID', 'DCCID:');
        $this->addBasicText('PSCID', 'PSCID:');
        $this->addSelect('gender', 'Gender:', array('' => 'All', 'Male' => 'Male', 'Female' => 'Female'));
        $this->addSelect('SubprojectID', 'Suproject:', array('' => 'All') + $list_of_subprojects);
        $this->addBasicText('dob', 'Date of Birth:', 'maxlength=10');
        $this->addBasicText('Visit_Count','Number of Visits:');
        $this->addSelect('Latest_Visit_Status','Latest Visit Status:', $latest_visit_status_options);
        $this->addSelect('Feedback','Feedback:', $feedback_status_options);
        $this->addSelect('Participant_Status','Participant Status:', $participant_status_options);
        $this->addSelect('scan_done','Scan Done:', $scan_options);
        $config=&NDB_Config::singleton();
        if($config->getSetting("useProjects")=="true"){
           $this->addSelect('ProjectID', 'Project:', array('' => 'All') + $list_of_projects);
        }
        if($config->getSetting("useEDC")=="true"){
        	$this->addBasicText('edc', 'EDC:', 'maxlength=10');
        	$this->tpl_data['useEDC']="true";
        }
        $this->addSelect('feedback', 'Feedback Status:', array_merge(array('' => 'All'), $feedback_status_options));

        return true;
    }


    function _setDataTableRows($count)
    {
        // create user object
        $user =& User::singleton();
        if(Utility::isErrorX($user)) {
            return PEAR::raiseError("User Error: ".$user->getMessage());
        }
		$subprojectlist=Utility::getSubprojectList();
        $projectlist=Utility::getProjectList();
        $participantOptions= NDB_Form_participant_status::getParticipantStatusOptions();
        // print out
        $x = 0;
        foreach ($this->list as $item) {
            //count column
            $this->tpl_data['items'][$x][0]['value'] = $x + $count;

            //print out data rows
            $i = 1;
            foreach ($item as $key => $val) { 
                //if it's a non-PSC user show the URL to the timepoint list on PSCID field
                if ($key == 'PSCID' && $user->hasPermission('access_all_profiles')) {
                    $this->tpl_data['items'][$x][$i]['DCCID'] = $item['DCCID'];
                }
                $this->tpl_data['items'][$x][$i]['name'] = $key;
                $this->tpl_data['items'][$x][$i]['value'] = $val;
                // get feedback information for each displayed candidate record
                if ($key == 'Feedback' && !empty($val)) {
                    // overwrite value
                    $this->tpl_data['items'][$x][$i]['value'] = NDB_BVL_Feedback::getFeedbackStatusVerbose($val);
                    $this->tpl_data['items'][$x][$i]['bgcolor'] = NDB_BVL_Feedback::getThreadColor($this->tpl_data['items'][$x][$i]['value']);
                } else if ($key=='Feedback' && empty($val)){
                	$this->tpl_data['items'][$x][$i]['value'] = "None";
                } else if ($key=='Project') {
                    if(isset($projectlist[$val])) {
                        $this->tpl_data['items'][$x][$i]['value'] = $projectlist[$val];
                    } else {
                        $this->tpl_data['items'][$x][$i]['value'] = '';
                    }
                } else if ($key=='Subproject') {
                    if(isset($subprojectlist[$val])) {
                        $this->tpl_data['items'][$x][$i]['value'] = $subprojectlist[$val];
                    } else {
                        $this->tpl_data['items'][$x][$i]['value'] = '';
                    }
                } else if ($key == 'Participant_Status' && !empty($val)) {
                	$this->tpl_data['items'][$x][$i]['value'] = $participantOptions[$val];
                }
                  else if ($key=='scan_done'){
                    $this->tpl_data['items'][$x][$i]['value'] = $val;
                }

                $i++;
            }

            $x++;
        }

        return true;
    }
}
?>
