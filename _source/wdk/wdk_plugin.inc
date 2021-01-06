<?php

	function IsValidPluginID($strPluginID)
	{
		if ($strPluginID == '')	
		{
			return false;	
		}
		if (StringCheckCharSet($strPluginID,CHARSET_PLUGINID) != true)
		{
			return false;	
		}
		if (StringCheckCharSet(StringSection($strPluginID,0,1),CHARSET_ALPHA_LOWERCASE) != true)
		{
			return false;
		}
		return true;
	}

	class CPlugin extends CWebsiteSatellite
	{
	
		function __construct(
			CWebSite &$website,
			$strPluginID)
		{
			if (IsValidPluginID($strPluginID) == false)
			{
				FatalError("CPlugin has been initialized with an invalid plugin id.");
			}
			parent::__construct($website,$strPluginID);
		}
		
		function GetSatelliteType()
		{
			return 'Plugin';	
		}
	
	
		function OnInit()
		{
			$this->Trace('OnInit');
			return true;
		}
		
		function OnCreateImage(&$image,$strImageID)
		{
			$this->Trace('OnCreateImage');
			return false;
		}
		
		function OnUserChangePassword($strOldPassword,$strNewPassword)
		{
			$this->Trace('OnUserChangePassword');
			return true;
		}
		
		function OnRenderGenericOutputItem($strItem, $arrayAttributes)
		{
			$this->Trace('OnRenderGenericOutputItem');
			return '';
		}

		
	}



