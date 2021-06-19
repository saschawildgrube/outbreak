<?php

	require_once(GetSourceDir().'assemblies/google/element_react_googlemaps.inc');

	class CGoogleMapsElement extends CGoogleMapsReactElement
	{
		function __construct(
			CWebSite $website,
			$strStyleID)
		{
			parent::__construct(
				$website,
				'googlemaps',
				$strStyleID,
				true,  
				false); 
		}
		
		function RenderGoogleMaps(
			$fLat, $fLong,
			$nZoom = 3,
			$strType = 'terrain',
			$bTypeControl = true,
			$bStreetViewControl = true,
			$arrayMarkers = array())
		{
			$arrayProps = array();
			$arrayProps['height'] = intval($this->GetLayoutDef('LAYOUT_MAP_HEIGHT'));
			$arrayProps['width'] = intval($this->GetLayoutDef('LAYOUT_MAP_WIDTH'));
			$arrayProps['lat'] = $fLat;
			$arrayProps['long'] = $fLong;
			$arrayProps['zoom'] = $nZoom;
			$arrayProps['type'] = $strType;
			$arrayProps['typecontrol'] = GetBoolValue($bTypeControl);
			$arrayProps['streetviewcontrol'] = GetBoolValue($bStreetViewControl);
			$arrayProps['markers'] = $arrayMarkers;
			return $this->RenderReactComponent(
				'GoogleMapsReactComponent',
				$arrayProps);
		}
	}

