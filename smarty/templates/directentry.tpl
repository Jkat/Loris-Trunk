<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<link rel="stylesheet" href="direct.css" type="text/css" />
<link rel="stylesheet" href="{$css}" type="text/css" />
<link rel="shortcut icon" href="images/mni_icon.ico" type="image/ico" />
<title>{$study_title}</title>

<link type="text/css" href="css/jquery-ui-1.8.10.custom.css" rel="Stylesheet" />	
<script src="js/jquery/jquery-1.11.0.min.js" type="text/javascript"></script>
<script type="text/javascript" src="js/jquery/jquery-ui-1.10.4.custom.min.js"></script>
<script type="text/javascript" src="js/modules/direct_entry.js"></script>

{if $test_name_js}
<script type="text/javascript" src="{$test_name_js}"></script>
{/if}
</head>

<body>
{if $dynamictabs neq "dynamictabs"}
<table border="0" cellpadding="3" cellspacing="2" width="100%" class="mainlayout">
    <tr>
        <th align="left" background="images/title_background.jpg" class="banner" colspan="2">
            {$study_title}
        </th>
    </tr>

    <tr>
    <!-- user info table -->
         <td width="50%" colspan="2" valign="bottom" align="left" nowrap="nowrap" class="controlPanelSection">
            Date: {$smarty.now|date_format:"%B %e %Y"}
            {if $finalpage || $complete}
            {elseif $pageNum && $totalPages}
            Page {$pageNum} of {$totalPages}
            {/if}
        </td>
    </tr>
    <tr>
        <!-- main page table tags -->
        <td width="100%" class="tabox" valign="top">

            <!-- Start workspace area -->
<!--h1 align="right"><a href="javascript:open_help_section()" ><u>Help</u></a>  </h1-->
{/if}
{if $error_message != ""}
            <p>The following errors occured while attempting to display this page:

            <ul>
    {section name=error loop=$error_message}
                <li><strong>{$error_message[error]}</strong></li>
    {/section}
            </ul>
            
            If this error persists, please report a bug using <a target="mantis" href="{$mantis_url}">Mantis</a>.</p>
            <p><a href="javascript:history.back(-1)">Please click here to go back</a>.</p>
{else}

        <!-- included file -->
{if $finalpage} 
{$workspace}
<h1 align="center">Review Page</h1>
<p>Please review the data entered below. Unanwered questions are highlighed in red. Please use the back button to go access the previous pages and enter any data that is missing.</p>
<p>Unanswered questions are highlighted in red.</p>
{if $review}
<hr>
<br><br>
{$review}
{/if}
<hr>
<form id="test_form" method="post">
<br>
<br>
<br>
<h1>Survey Comments</h1>
<br>
<br>
<table class="instrument">
    <tr>
        <td>How would you rate the ease of filling out this survey?</td>
        <td>
            <input type="radio" name="ease" value="1"> Very easy
            <input type="radio" name="ease" value="2"> Moderately easy
            <input type="radio" name="ease" value="3"> Average
            <input type="radio" name="ease" value="4"> Moderately difficult
            <input type="radio" name="ease" value="5"> Difficult
    </tr>
    <tr>
        <td>Do you have any other comments ?</td>
        <td><textarea name="comments" id="comments" rows="5" placeholder="Enter any comments about this survey here"></textarea></td>
    </tr>
    <tr>
        <td class="note" colspan="2">
            <b>Please note that once data is submitted you will not be able to modify it.</b>
        </td>
    </tr>
</table>
<input type="hidden" name="FinalPageSubmission" value="Yes">
<input type="button" onclick="location.href='survey.php?key={$key}&pageNum={$prevpage}'" value="Go Back">
<button id="complete" style="font-weight: bold;">
<span style="display: none" id="key">{$key}</span>
Submit data
</button>
</form>
{else}
<table align="center">
<tr>
    <td>
            
        {$workspace}
    </td>
</tr>
{if ($nextpage || $prevpage) && !$complete}
{* This is defined here but moved to the instrument table
   through javascript *}
<tr id="buttons">
    <td colspan="2">
    {if $prevpage}
        {if $prevpage eq 'top'}
            <button id="goback">Save and Go Back</button>
        {else}
            <button id="goback">Save and Go Back</button>
        {/if}
        <span style="display: none" id="prevpage">{$prevpage}</span>
    {/if}

    {if $nextpage} 
    <button id="savecontinue">
        <span style="display: none" id="nextpage">{$nextpage}</span>
        <span style="display: none" id="key">{$key}</span>
        Save and Continue
    </button>
    {else}
    <button id="finalize">
        <span style="display: none" id="key">{$key}</span>
        Save and Finalize
    </button>
    {/if}
    </td>
</tr>
{/if} 
</table>
{/if}
        </td>
    </tr>
</table>
{/if}

</body>
</html>
