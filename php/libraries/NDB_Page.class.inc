<?php

require_once "PEAR.php";
require_once "HTML/QuickForm.php";
include_once ('HTML/QuickForm/Renderer/Default.php');

class NDB_Page extends PEAR
{
    function _setupPage($name, $page, $identifier, $commentID, $formname) {
        $this->form = new HTML_QuickForm($formname);
        $this->name = $name;
        $this->page = $page;
        $this->identifier = $identifier;
        $this->commentID = $commentID;
    }
    
    
    /**
    * Wrapper to create a file element
    */
    function addFile($name, $label) {
        $this->form->addElement('file', $name, $label);
    }

     /**
    * Wrapper to create a header
    */

    function addHeader($header) {
        $this->form->addElement('header', null, $header);
    }

    /**
    * Wrapper to create a select drop-down list
    */

    function addSelect($name, $label, $options, $optional=null) {
        $this->form->addElement('select', $name, $label, $options, $optional);
    }

    /**
    * Wrapper to create a static label
    */

    function addLabel($label) {
        $this->form->addElement('static', null, $label);
    }

    /**
    * Wrapper to create a static score column
    */

    function addScoreColumn($name, $label) {
        $this->form->addElement('static', $name, $label);
    }


    function addBasicText($field, $label, $options=array()) {
        $this->form->addElement('text', $field, $label, $options);
    }

    function addBasicTextArea($field, $label, $specifications=array()) {
        $this->form->addElement('textarea', $field, $label, $specifications);
    }

    function addBasicDate($field, $label, $options=array()) {
        $this->form->addElement('date', $field, $label, $options);
    }
 
    function addHidden($label, $attributes=null) {
        $this->form->addElement('hidden', $label, $attributes);
    }

    function addTextAreaGroup($field, $label,$not_answered_options=array('' =>'', 'not_answered' => 'Not Answered')) {
        $group = array();
        $group[] = $this->createTextArea($field, '');
        $group[] = $this->createSelect($field . '_status', '', $not_answered_options);

        $this->addGroup($group, $field . '_group', $label, null, false); 


    }
    function addRule($element, $message, $type, $format=null) {
        $this->form->addRule($element, $message, $type, $format);
    }

    function addGroupRule($group, $arg1, $type='', $format=null) {
        $this->form->addGroupRule($group, $arg1, $type, $format);
    }

    function addGroup($elements, $name, $label, $separator, $appendName=null) {
        $this->form->addGroup($elements, $name, $label, $separator, $appendName);
    }

    function createSelect($field, $label, $options=null) {
	return $this->form->createElement("select", $field, $label, $options);
    }

    function createText($field, $label=null) {
       return $this->form->createElement("text", $field, $label);
    } 

    function createTextArea($field, $label=null) {
       return $this->form->createElement("textarea", $field, $label);
    } 
    function createDate($field, $label, $dateOptions=null) {
        return $this->form->createElement("date", $field, $label, $dateOptions);
    }
  

    /**
     * Displays the form
     *
     * @return void
     * @access public
     */
    function display()
    {
        if(!$this->form->isFrozen()) {
            // hidden values
              $this->addHidden('test_name', $this->name);
              $this->addHidden('subtest', $this->page);
              $this->addHidden('identifier', $this->identifier);

           // $this->form->addElement('hidden', 'test_name', $this->name);
           // $this->form->addElement('hidden', 'subtest', $this->page);
           // $this->form->addElement('hidden', 'identifier', $this->identifier);
         }

        // get the defaults
        $localDefaults = $this->_getDefaults();
        if (PEAR::isError($localDefaults)) {
            return PEAR::raiseError("NDB_Form::display(): ".$localDefaults->getMessage());
        }
        if(!is_array($localDefaults)) { $localDefaults = array(); }
        // set the quickform object defaults
        $this->form->setDefaults(array_merge($this->defaults, $localDefaults));

            // trim all values
        $this->form->applyFilter('__ALL__', 'trim');

        // display the HTML_Quickform object
        $smarty = new Smarty_neurodb;
        $renderer = new HTML_QuickForm_Renderer_ArraySmarty($smarty);
        $this->form->accept($renderer);
        $smarty->assign('form', $renderer->toArray());
        $smarty->assign($this->tpl_data);
        $html = $smarty->fetch("form_$this->template.tpl");
        return $html;
        //return $this->form->toHTML();
    }


    /**
     * Returns the form-specific defaults
     *
     * @note   overloaded function
     * @return array
     * @access private
     */
    function _getDefaults()
    {
        return array();
    }

}
?>
