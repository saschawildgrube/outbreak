<?php

	require_once(GetWDKDir().'wdk_element_react.inc');

	abstract class CGoogleMapsReactElement extends CReactElement
	{
		function __construct(
			CWebSite &$website,
			$strElementID,
			$strStyleID,
			$bLayoutCFG,
			$bLayoutHTML)
		{
			parent::__construct(
				$website,
				$strElementID,
				$strStyleID,
				$bLayoutCFG,
				$bLayoutHTML);
			
			$strApiKey = $this->GetWebsite()->GetConfig("googlemaps_apikey");
			$this->AddJavaScriptURL('googlemaps','https://maps.googleapis.com/maps/api/js?key='.$strApiKey); 
			$this->AddJavaScript('react-googlemaps'); 
		}
	}

