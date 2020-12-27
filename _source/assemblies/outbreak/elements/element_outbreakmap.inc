<?php

	require_once(GetSourceDir().'assemblies/google/elements/element_googlemaps.inc');

	class COutbreakMapElement extends CGoogleMapsElement
	{
		function __construct(
			CWebSite $website,
			$strStyleID)
		{
			parent::__construct(
				$website,
				'outbreakmap',
				$strStyleID,
				true,  
				false); 
				
			$this->GetWebsite()->ActivateD3v5();
			$this->AddJavaScript('wdk-react-d3'); 
		}
		
		function RenderOutbreakMap($arrayData)
		{
			$arrayProps = array();
			$arrayProps['height'] = intval($this->GetLayoutDef('LAYOUT_MAP_HEIGHT'));
			$arrayProps['width'] = intval($this->GetLayoutDef('LAYOUT_MAP_WIDTH'));
			$arrayProps['data'] = $arrayData;
			$this->AddJavaScript('react_outbreakmap');
			return $this->RenderReactComponent(
				'OutbreakMapReactComponent',
				$arrayProps);
		}
	}

