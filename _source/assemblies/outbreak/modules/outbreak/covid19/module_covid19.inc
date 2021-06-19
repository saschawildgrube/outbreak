<?php

	require_once(GetWDKDir().'wdk_module.inc');
	require_once(GetWDKDir().'wdk_cache.inc');
	require_once(GetAssemblyDir('outbreak').'covid19.inc');

	
	class CCOVID19Module extends CModule
	{
		private $m_arrayOutbreakData;
		private $m_cache;
		private $m_bLoading;
	
		function __construct(
			CWebSite $website,
			$strInstanceID = '',
			$arrayConfig = array())
		{
			$this->m_arrayOutbreakData = null;
			$this->m_cache = new CFileCache(GetTempDir(),'outbreak-'.GetEnvID());
			$this->m_bLoading = false;

			parent::__construct(
				$website,
				'outbreak/covid19',
				$strInstanceID,
				$arrayConfig);
		}
		
		function OnInit()
		{
			$this->LoadStringResources('local');
			return parent::OnInit();
		}
		
	
		function OnRenderModule($strState)
		{
			$this->Trace('OnRenderModule("'.$strState.'")');
				
			if ($this->LoadDataFromCache() == false)
			{
				return $this->StateLoading();	
			}

			$this->SetOutputItem('SOURCES',$this->RenderSources());
						
			switch ($strState)
			{
				case 'loading':
					return $this->StateLoading();
				case 'country':
					return $this->StateCountry();
				case 'countrylist':
					return $this->StateCountryList();
				case '':
					$this->SetState('worldmap');
				case 'worldmap':
				default:
					return $this->StateWorldMap();
			}
		}
		
		function LoadDataFromCache()
		{
			if (IsArray($this->m_arrayOutbreakData) == true)
			{
				$this->Trace('Outbreak data is already loaded.');
				return true;	
			}
			$arrayOutbreakData = $this->m_cache->GetCacheObject('outbreak');
			if (IsArray($arrayOutbreakData) == true)
			{
				$this->Trace('Outbreak data loaded from cache.');
				$this->m_arrayOutbreakData = $arrayOutbreakData;
				return true;
			}
			if (GetBoolValue($this->GetModuleVar('forceload')) == true)
			{
				$this->SetModuleVar('forceload',false);
				$this->m_cache->SetCacheObject('outbreak',$this->GetOutbreakData(),60*60);
				$this->Trace('Outbreak data stored in cache.');
				return true;
			}
			return false;	
		}
		
		function GetOutbreakData()
		{
			$this->Trace('GetOutbreakData');
			if (!IsArray($this->m_arrayOutbreakData))
			{
				$arrayOutbreakData = GetOutbreakDataCovid19();
	
				$arrayCountries = ArrayGetValue($arrayOutbreakData,'countries');
				foreach ($arrayCountries as &$arrayCountry)
				{
					$strCountryID = ArrayGetValue($arrayCountry,'id');
					if (IsValidCountryID($strCountryID))
					{
						$arrayCountry['name'] = $this->LoadString('TID_COUNTRY_'.$strCountryID);	
					}
					
					$nPopulation = GetIntegerValue(ArrayGetValue($arrayCountry,'population'));
					$nConfirmed = GetIntegerValue(ArrayGetValue($arrayCountry,'confirmed'));	
					$nDeaths = GetIntegerValue(ArrayGetValue($arrayCountry,'deaths'));
					$nRecovered = GetIntegerValue(ArrayGetValue($arrayCountry,'recovered'));
					$nActive = GetIntegerValue(ArrayGetValue($arrayCountry,'active'));
					$fMortality = GetFloatValue(ArrayGetValue($arrayCountry,'mortality'));
	
					$strConfirmed = $this->GetWebsite()->RenderNumber($nConfirmed);	
					$strDeaths = $this->GetWebsite()->RenderNumber($nDeaths);
					$strRecovered = $this->GetWebsite()->RenderNumber($nRecovered);
					$strActive = $this->GetWebsite()->RenderNumber($nActive);

	
					$strMortality = 'n/a';
					if ($nConfirmed > 1000)
					{
						$strMortality = $this->GetWebsite()->RenderNumber(
							$fMortality,
							2,
							'%');
					}

					//$arrayCountry['mortality'] = $fMortality;
					$arrayCountry['text_confirmed'] = $strConfirmed;
					$arrayCountry['text_deaths'] = $strDeaths;
					$arrayCountry['text_recovered'] = $strRecovered;
					$arrayCountry['text_active'] = $strActive;
					$arrayCountry['text_mortality'] = $strMortality;
					
					if ($nPopulation > 1000)
					{
						$strPopulation = $this->GetWebsite()->RenderNumber($nPopulation);
						$strConfirmedPerCapita = $this->GetWebsite()->RenderNumber(ArrayGetValue($arrayCountry,'confirmedpercapita'),2);
						$strDeathsPerCapita = $this->GetWebsite()->RenderNumber(ArrayGetValue($arrayCountry,'deathspercapita'),2);
						$strRecoveredPerCapita = $this->GetWebsite()->RenderNumber(ArrayGetValue($arrayCountry,'recoveredpercapita'),2);
						$strActivePerCapita = $this->GetWebsite()->RenderNumber(ArrayGetValue($arrayCountry,'activepercapita'),2);
					}
					else
					{
						$strPopulation = 'n/a';	
						$strConfirmedPerCapita = 'n/a';
						$strDeathsPerCapita = 'n/a';
						$strRecoveredPerCapita = 'n/a';
						$strActivePerCapita = 'n/a';
					}					
					$arrayCountry['text_population'] = $strPopulation;
					$arrayCountry['text_confirmedpercapita'] = $strConfirmedPerCapita;
					$arrayCountry['text_deathspercapita'] = $strDeathsPerCapita;
					$arrayCountry['text_recoveredpercapita'] = $strRecoveredPerCapita;
					$arrayCountry['text_activepercapita'] = $strActivePerCapita;
					
					$arrayParams = array();
					$arrayParams[$this->GetModuleVarNameState()] = 'country';
					$arrayParams[$this->GetModuleVarName('country')] = $strCountryID;
					$strLinkCountry = $this->MakeURL($arrayParams,false);
					
					$arrayCountry['link'] = $strLinkCountry;
					
				}
				$arrayOutbreakData['countries'] = $arrayCountries;

				$arrayShips = ArrayGetValue($arrayOutbreakData,'ships');
				foreach ($arrayShips as &$arrayShip)
				{
					$nConfirmed = GetIntegerValue(ArrayGetValue($arrayShip,'confirmed'));	
					$nDeaths = GetIntegerValue(ArrayGetValue($arrayShip,'deaths'));
					$nRecovered = GetIntegerValue(ArrayGetValue($arrayShip,'recovered'));
					$nActive = GetIntegerValue(ArrayGetValue($arrayShip,'active'));

					$fMortality = GetFloatValue(ArrayGetValue($arrayShip,'mortality'));
	
					$strConfirmed = $this->GetWebsite()->RenderNumber($nConfirmed);	
					$strDeaths = $this->GetWebsite()->RenderNumber($nDeaths);
					$strRecovered = $this->GetWebsite()->RenderNumber($nRecovered);
					$strActive = $this->GetWebsite()->RenderNumber($nActive);
	
					$strMortality = 'n/a';
					if ($nConfirmed > 100)
					{
						$strMortality = $this->GetWebsite()->RenderNumber(
							$fMortality,
							2,
							'%');
					}

					$arrayShip['text_confirmed'] = $strConfirmed;
					$arrayShip['text_deaths'] = $strDeaths;
					$arrayShip['text_recovered'] = $strRecovered;
					$arrayShip['text_active'] = $strActive;
					$arrayShip['text_mortality'] = $strMortality;
				}
								
				$arrayOutbreakData['ships'] = $arrayShips;
	
				$arrayWorld = ArrayGetValue($arrayOutbreakData,'world');
				if (is_array($arrayWorld))
				{
					$nConfirmed = GetIntegerValue(ArrayGetValue($arrayWorld,'confirmed'));	
					$nDeaths = GetIntegerValue(ArrayGetValue($arrayWorld,'deaths'));
					$nRecovered = GetIntegerValue(ArrayGetValue($arrayWorld,'recovered'));
					$nActive = GetIntegerValue(ArrayGetValue($arrayWorld,'active'));
					$fMortality = GetFloatValue(ArrayGetValue($arrayWorld,'mortality'));
		
					$strConfirmed = $this->GetWebsite()->RenderNumber($nConfirmed);	
					$strDeaths = $this->GetWebsite()->RenderNumber($nDeaths);
					$strRecovered = $this->GetWebsite()->RenderNumber($nRecovered);
					$strActive = $this->GetWebsite()->RenderNumber($nActive);
					
					$strMortality = 'n/a';
					if ($nConfirmed > 10000)
					{
						
						$strMortality = $this->GetWebsite()->RenderNumber(
							$fMortality,
							2,
							'%');
					}
	
					$arrayWorld['mortality'] = $fMortality;
					$arrayWorld['text_confirmed'] = $strConfirmed;
					$arrayWorld['text_deaths'] = $strDeaths;
					$arrayWorld['text_recovered'] = $strRecovered;
					$arrayWorld['text_active'] = $strActive;
					$arrayWorld['text_mortality'] = $strMortality;
					
					$arrayOutbreakData['world'] = $arrayWorld;
				}
				$this->m_arrayOutbreakData = $arrayOutbreakData;	
			}
			//$this->Trace($this->m_arrayOutbreakData);
			return $this->m_arrayOutbreakData;
		}
		
		function RenderSummary()
		{
			if ($this->m_bLoading == true)
			{
				return $this->LoadString('TID_INFO_LOADING');	
			}
			$arrayOutbreakData = $this->GetOutbreakData();

			$strSummary = '';
			
			$strConfirmed = ArrayGetValue($arrayOutbreakData,'world','text_confirmed');
			$strDeaths = ArrayGetValue($arrayOutbreakData,'world','text_deaths');
			//$strRecovered = ArrayGetValue($arrayOutbreakData,'world','text_recovered');
			//$strActive = ArrayGetValue($arrayOutbreakData,'world','text_active');
			$strMortality = ArrayGetValue($arrayOutbreakData,'world','text_mortality');	
	
			$strSummary .=
				'?TID_LABEL_CONFIRMED?: '.$strConfirmed
			.	' / ?TID_LABEL_DEATHS?: '.$strDeaths			
			.	' / ?TID_LABEL_MORTALITY?: '.$strMortality
			//. ' / ?TID_LABEL_RECOVERED?: '.$strRecovered
			//. ' / ?TID_LABEL_ACTIVE?: '.$strActive
			;

			return $strSummary;	
		}
		
		function RenderSources()
		{
			$arrayOutbreakData = $this->GetOutbreakData();

			$arraySources = ArrayGetValue($arrayOutbreakData,'sources');
			if (!is_array($arraySources))
			{
				$arraySources = array();
			}
			
			$strSources = '';
			foreach ($arraySources as $arraySource)
			{
				//$strSources .= ArrayGetValue($arraySource,'name').': '.ArrayGetValue($arraySource,'url').'<br/>';
				$strSources .= ArrayGetValue($arraySource,'name').'<br/>';
			}
			
			$strSources .=
				'?TID_LABEL_LATESTDATA?: '
				.$this->RenderDate(GetTime(ArrayGetValue($arrayOutbreakData,'date_latestdata')))
				.' (?TID_LABEL_LOADED?: '
				.$this->RenderDateTime(GetTime(ArrayGetValue($arrayOutbreakData,'datetime_downloaded')))
				.')';
			return $strSources;
		}

		function StateLoading()
		{
			if ($this->m_bLoading == false)
			{
				$arrayParams = array();
				$arrayParams[$this->GetModuleVarName('forceload')] = '1';
				$strURL = $this->MakeURL($arrayParams);
				$this->GetWebsite()->Redirect($strURL);
			}
			$this->m_bLoading = true;

			$this->IncludeElement('loading');
			$loading = new CLoadingElement($this->GetWebsite(),'default');
			$this->SetOutputItem(
				'LOADING',
				$loading->RenderLoading());

			return 'loading';	
		}
		
		function StateWorldMap()
		{
			
			$arrayOutbreakData = $this->GetOutbreakData();
			$arrayOutbreakData = ArrayRemoveKeyRecursive($arrayOutbreakData,'timeseries');
			
			$arrayMarkers = [];
			
			$arrayCountries = ArrayGetValue($arrayOutbreakData,'countries');
			
			$fIncidenceMax = 0;
			foreach ($arrayCountries as $arrayCountry)
			{
				$fIncidence = GetFloatValue(ArrayGetValue($arrayCountry,'incidence_7days'));
				$fIncidenceMax = max($fIncidenceMax,$fIncidence);
			}
			
			if ($fIncidenceMax <= 0)
			{
				$this->Trace('The maximum seven days incidence is zero? Is the pandemic over?');
				$fIncidenceMax = 1.0;
			}


			foreach ($arrayCountries as $arrayCountry)
			{
				$arrayMarker = [];
				
				$arrayMarker['long'] = ArrayGetValue($arrayCountry,'long');
				$arrayMarker['lat'] = ArrayGetValue($arrayCountry,'lat');
				
				$fIncidence = GetFloatValue(ArrayGetValue($arrayCountry,'incidence_7days'));
				
				$strToolTip =
					ArrayGetValue($arrayCountry,'name') . "\n"
					.$this->LoadString('TID_LABEL_INCIDENCE_7DAYS').': '.RenderNumber($fIncidence,1)
					;
					
				$strColor = 'green';
				if ($fIncidence > 50)
				{
					$strColor = 'orange';	
				}
				if ($fIncidence > 100)
				{
					$strColor = 'red';	
				} 
				
				$arrayMarker['tooltip'] = $strToolTip;
				$arrayMarker['color'] = $strColor;
				$arrayMarker['opacity'] = 0.5;
				$arrayMarker['size'] = $fIncidence / $fIncidenceMax;

				$arrayMarker['link'] = ArrayGetValue($arrayCountry,'link');
				
				$arrayMarkers[] = $arrayMarker;
			}
			
			
			$this->IncludeElement('googlemaps');
			$googlemaps = new CGoogleMapsElement($this->GetWebsite(),'default');
			$this->SetOutputItem(
				'WORLDMAP',
				$googlemaps->RenderGoogleMaps(
					30,0,
					2,'satellite',  
					false,
					false,
					$arrayMarkers));
			
			
			
			
			$this->IncludeElement('legend');
			$legend = new CLegendElement($this->GetWebsite(),'default'); 
			
			$arrayWorldmapLegend = array(
					array(
						'label' => '?TID_LABEL_INCIDENCE_7DAYS? < 50',
						'color' => 'green'
						),
					array(
						'label' => '50 < ?TID_LABEL_INCIDENCE_7DAYS? < 100',
						'color' => 'orange' 
						),
					array(
						'label' => '?TID_LABEL_INCIDENCE_7DAYS? > 100',
						'color' => 'red'
						)
					);
						
			$this->SetOutputItem(
				'WORLDMAP_LEGEND',
				$legend->RenderLegend(
					$arrayWorldmapLegend));
		
			
			return 'worldmap';
		}


		function StateWorldMapConfirmed()
		{
			
			$arrayOutbreakData = $this->GetOutbreakData();
			$arrayOutbreakData = ArrayRemoveKeyRecursive($arrayOutbreakData,'timeseries');
			//$arrayOutbreakData = ArrayRemoveKeyRecursive($arrayOutbreakData,'active');
			//$arrayOutbreakData = ArrayRemoveKeyRecursive($arrayOutbreakData,'recovered');

			
			$this->IncludeElement('outbreakmap');
			$outbreakmap = new COutbreakMapElement($this->GetWebsite(),'default');
			$this->SetOutputItem('WORLDMAP',$outbreakmap->RenderOutbreakMap($arrayOutbreakData));
			
			
			
			
			$this->IncludeElement('legend');
			$legend = new CLegendElement($this->GetWebsite(),'default'); 
			
			$arrayWorldmapLegend = array(
					array(
						'label' => '?TID_LABEL_WORLDMAP_DEATHS?',
						'color' => 'red'
						),
					array(
						'label' => '?TID_LABEL_WORLDMAP_CONFIRMED?',
						'color' => 'orange' 
						),
					array(
						'label' => '?TID_LABEL_WORLDMAP_NONE?',
						'color' => 'green'
						)/*,
					array(
						'label' => '?TID_LABEL_WORLDMAP_COUNTRY?',
						'color' => 'grey',
						'icon' => 'circle'
						),
					array(
						'label' => '?TID_LABEL_WORLDMAP_SHIP?',
						'color' => 'grey',
						'icon' => 'arrow-up'
						)*/
					);
						
			$this->SetOutputItem(
				'WORLDMAP_LEGEND',
				$legend->RenderLegend(
					$arrayWorldmapLegend));
		
			
			return 'worldmap';
		}

		
		function StateCountryList()
		{

			$strID = $this->GetModuleVar('country');
			if ($strID != '')
			{
				return $this->StateCountry();	
			}

			$arrayOutbreakData = $this->GetOutbreakData();
			$arrayCountries = ArrayGetValue($arrayOutbreakData,'countries');
			
			function CompareCountriesByName($arrayCountry1,$arrayCountry2)
			{
				return CompareString(ArrayGetValue($arrayCountry1,'name'),ArrayGetValue($arrayCountry2,'name'));	
			}
			function CompareCountriesByPopulation($arrayCountry1,$arrayCountry2)
			{
				return
					GetIntegerValue(ArrayGetValue($arrayCountry2,'population'))
					-
					GetIntegerValue(ArrayGetValue($arrayCountry1,'population'));	
			}
			function CompareCountriesByConfirmedPerCapita($arrayCountry1,$arrayCountry2)
			{
				$nPopulation1 = GetIntegerValue(ArrayGetValue($arrayCountry1,'population'));
				$nPopulation2 = GetIntegerValue(ArrayGetValue($arrayCountry2,'population')); 
				$fConfirmedPerCapita1 = GetFloatValue(ArrayGetValue($arrayCountry1,'confirmedpercapita'));
				$fConfirmedPerCapita2 = GetFloatValue(ArrayGetValue($arrayCountry2,'confirmedpercapita'));
				if ($nPopulation1 >= 1000 && $nPopulation2 >= 1000)
				{
					return GetIntegerValue(
						($fConfirmedPerCapita2 * 1000)
						- ($fConfirmedPerCapita1 * 1000)
						);	
				}
				if ($nPopulation1 >= 1000)
				{
					return -1;	
				}
				if ($nPopulation2 >= 1000)
				{
					return 1;	    
				}
				return 0;
			}
			function CompareCountriesByDeathsPerCapita($arrayCountry1,$arrayCountry2)
			{
				$nPopulation1 = GetIntegerValue(ArrayGetValue($arrayCountry1,'population'));
				$nPopulation2 = GetIntegerValue(ArrayGetValue($arrayCountry2,'population')); 
				$fDeathsPerCapita1 = GetFloatValue(ArrayGetValue($arrayCountry1,'deathspercapita'));
				$fDeathsPerCapita2 = GetFloatValue(ArrayGetValue($arrayCountry2,'deathspercapita'));
				if ($nPopulation1 >= 1000 && $nPopulation2 >= 1000)
				{
					return GetIntegerValue(
						($fDeathsPerCapita2 * 1000)
						- ($fDeathsPerCapita1 * 1000)
						);	
				}
				if ($nPopulation1 >= 1000)
				{
					return -1;	
				}
				if ($nPopulation2 >= 1000)
				{
					return 1;	    
				}
				return 0;
			}
			function CompareCountriesByRecoveredPerCapita($arrayCountry1,$arrayCountry2)
			{
				$nPopulation1 = GetIntegerValue(ArrayGetValue($arrayCountry1,'population'));
				$nPopulation2 = GetIntegerValue(ArrayGetValue($arrayCountry2,'population')); 
				$fRecoveredPerCapita1 = GetFloatValue(ArrayGetValue($arrayCountry1,'recoveredpercapita'));
				$fRecoveredPerCapita2 = GetFloatValue(ArrayGetValue($arrayCountry2,'recoveredpercapita'));
				if ($nPopulation1 >= 1000 && $nPopulation2 >= 1000)
				{
					return GetIntegerValue(
						($fRecoveredPerCapita2 * 1000)
						- ($fRecoveredPerCapita1 * 1000)
						);	
				}
				if ($nPopulation1 >= 1000)
				{
					return -1;	
				}
				if ($nPopulation2 >= 1000)
				{
					return 1;	    
				}
				return 0;
			}
			function CompareCountriesByActivePerCapita($arrayCountry1,$arrayCountry2)
			{
				$nPopulation1 = GetIntegerValue(ArrayGetValue($arrayCountry1,'population'));
				$nPopulation2 = GetIntegerValue(ArrayGetValue($arrayCountry2,'population')); 
				$fActivePerCapita1 = GetFloatValue(ArrayGetValue($arrayCountry1,'activepercapita'));
				$fActivePerCapita2 = GetFloatValue(ArrayGetValue($arrayCountry2,'activepercapita'));
				if ($nPopulation1 >= 1000 && $nPopulation2 >= 1000)
				{
					return GetIntegerValue(
						($fActivePerCapita2 * 1000)
						- ($fActivePerCapita1 * 1000)
						);	
				}
				if ($nPopulation1 >= 1000)
				{
					return -1;	
				}
				if ($nPopulation2 >= 1000)
				{
					return 1;	    
				}
				return 0;
			}
			function CompareCountriesByConfirmed($arrayCountry1,$arrayCountry2)
			{
				return
					GetIntegerValue(ArrayGetValue($arrayCountry2,'confirmed'))
					-
					GetIntegerValue(ArrayGetValue($arrayCountry1,'confirmed'));	
			}
			function CompareCountriesByDeaths($arrayCountry1,$arrayCountry2)
			{
				return
					GetIntegerValue(ArrayGetValue($arrayCountry2,'deaths'))
					-
					GetIntegerValue(ArrayGetValue($arrayCountry1,'deaths'));	
			}
			function CompareCountriesByMortality($arrayCountry1,$arrayCountry2)
			{
				$nConfirmed1 = GetIntegerValue(ArrayGetValue($arrayCountry1,'confirmed'));
				$nConfirmed2 = GetIntegerValue(ArrayGetValue($arrayCountry2,'confirmed')); 
				$fMortality1 = GetFloatValue(ArrayGetValue($arrayCountry1,'mortality'));
				$fMortality2 = GetFloatValue(ArrayGetValue($arrayCountry2,'mortality'));
				if ($nConfirmed1 >= 1000 && $nConfirmed2 >= 1000)
				{
					return GetIntegerValue(
						($fMortality2 * 100)
						- ($fMortality1 * 100)  
						);
				}
				if ($nConfirmed1 >= 1000)
				{
					return -1;	
				}
				if ($nConfirmed2 >= 1000)
				{
					return 1;	    
				}
				return 0;
			}
			function CompareCountriesByRecovered($arrayCountry1,$arrayCountry2)
			{
				return
					GetIntegerValue(ArrayGetValue($arrayCountry2,'recovered'))
					-
					GetIntegerValue(ArrayGetValue($arrayCountry1,'recovered'));	
			}
			function CompareCountriesByActive($arrayCountry1,$arrayCountry2)
			{
				return
					GetIntegerValue(ArrayGetValue($arrayCountry2,'active'))
					-
					GetIntegerValue(ArrayGetValue($arrayCountry1,'active'));	
			}
			
			$strSortOption = $this->GetModuleVar('sort');
			$strSortFunction = '';
			if ($strSortOption == 'name')
			{
				$strSortFunction = 'CompareCountriesByName';				
			}
			if ($strSortOption == 'population')
			{
				$strSortFunction = 'CompareCountriesByPopulation';				
			}
			if ($strSortOption == 'confirmedpercapita')
			{
				$strSortFunction = 'CompareCountriesByConfirmedPerCapita';				
			}
			if ($strSortOption == 'deathspercapita')
			{
				$strSortFunction = 'CompareCountriesByDeathsPerCapita';
			}
			if ($strSortOption == 'recoveredpercapita')
			{
				$strSortFunction = 'CompareCountriesByRecoveredPerCapita';
			}
			if ($strSortOption == 'activepercapita')
			{
				$strSortFunction = 'CompareCountriesByActivePerCapita';
			}
			if ($strSortOption == 'confirmed')
			{
				$strSortFunction = 'CompareCountriesByConfirmed';
			}
			if ($strSortOption == 'deaths')
			{
				$strSortFunction = 'CompareCountriesByDeaths';
			}
			if ($strSortOption == 'recovered')
			{
				$strSortFunction = 'CompareCountriesByRecovered';
			}
			if ($strSortOption == 'active')
			{
				$strSortFunction = 'CompareCountriesByActive';
			}
			if ($strSortOption == 'mortality')
			{
				$strSortFunction = 'CompareCountriesByMortality';
			}
			
			// default
			if ($strSortFunction == '')
			{
				$strSortFunction = 'CompareCountriesByConfirmedPerCapita';
			}
			
			$arrayCountries = ArraySort($arrayCountries,false,$strSortFunction);
			
			$table = new CTableElement($this->GetWebsite(),'default');
			
			$arrayTable = array();
			$arrayHeaders = array();
			$arrayCellStyle = array();

			$link = new CLinkElement($this->GetWebsite(),'default');

			$strSortParamName = $this->GetModuleVarName('sort');
			$strLinkSortByCountry = $link->RenderLink(
				$this->GetWebsite()->MakeURL(array($strSortParamName => 'name')),
				'?TID_LABEL_COUNTRY?');

			$strLinkSortByPopulation = $link->RenderLink(
				$this->GetWebsite()->MakeURL(array($strSortParamName => 'population')),
				'?TID_LABEL_POPULATION?');
			
			$strLinkSortByConfirmedPerCapita = $link->RenderLink(
				$this->GetWebsite()->MakeURL(array($strSortParamName => 'confirmedpercapita')),
				'?TID_LABEL_CONFIRMEDPERCAPITA?');

			$strLinkSortByDeathsPerCapita = $link->RenderLink(
				$this->GetWebsite()->MakeURL(array($strSortParamName => 'deathspercapita')),
				'?TID_LABEL_DEATHSPERCAPITA?');
/*
			$strLinkSortByRecoveredPerCapita = $link->RenderLink(
				$this->GetWebsite()->MakeURL(array($strSortParamName => 'recoveredpercapita')),
				'?TID_LABEL_RECOVEREDPERCAPITA?');

			$strLinkSortByActivePerCapita = $link->RenderLink(
				$this->GetWebsite()->MakeURL(array($strSortParamName => 'activepercapita')),
				'?TID_LABEL_ACTIVEPERCAPITA?');
*/

			$strLinkSortByConfirmed = $link->RenderLink(
				$this->GetWebsite()->MakeURL(array($strSortParamName => 'confirmed')),
				'?TID_LABEL_CONFIRMED?');

			$strLinkSortByDeaths = $link->RenderLink(
				$this->GetWebsite()->MakeURL(array($strSortParamName => 'deaths')),
				'?TID_LABEL_DEATHS?');
/*
			$strLinkSortByRecovered = $link->RenderLink(
				$this->GetWebsite()->MakeURL(array($strSortParamName => 'recovered')),
				'?TID_LABEL_RECOVERED?');

			$strLinkSortByActive = $link->RenderLink(
				$this->GetWebsite()->MakeURL(array($strSortParamName => 'active')),
				'?TID_LABEL_ACTIVE?');
*/
			$strLinkSortByMortality = $link->RenderLink(
				$this->GetWebsite()->MakeURL(array($strSortParamName => 'mortality')),
				'?TID_LABEL_MORTALITY?');


		
			$this->IncludeElement('flag');
			$flag = new CFlagElement($this->GetWebsite(),'default');
			$link = new CLinkElement($this->GetWebsite(),'default');


			$strTab = $this->GetModuleVar('countrylist_tab');

			$this->IncludeElement('tabnavigation');
			$tabnavigation = new CTabNavigationElement($this->GetWebsite(),'default');

			switch ($strTab)
			{
			case 'absolute':
				$arrayTable[] = array(
					$strLinkSortByCountry,
					$strLinkSortByPopulation,
					$strLinkSortByConfirmed,
					$strLinkSortByDeaths,
					$strLinkSortByMortality,
					/*$strLinkSortByRecovered,
					$strLinkSortByActive*/);
				$arrayCellStyle[] = array('left','left','left','left','left'/*,'left','left'*/);  
				$arrayHeaders[] = array(true,true,true,true,true/*,true,true*/);
				
				foreach ($arrayCountries as $arrayCountry)
				{
					$strName = ArrayGetValue($arrayCountry,'name');
					$strID = ArrayGetValue($arrayCountry,'id');
					if (IsValidCountryID($strID))
					{
						$strName = $flag->RenderFlag($strID) . $strName;
					}
		
					$arrayParams = array();
					$arrayParams[$this->GetModuleVarNameState()] = 'country';
					$arrayParams[$this->GetModuleVarName('country')] = $strID;
					$strCountryURL = $this->GetWebsite()->MakeURL($arrayParams);
					$strName = $link->RenderLink($strCountryURL,$strName);
		
					$arrayTable[] = array(
						$strName,
						ArrayGetValue($arrayCountry,'text_population'),
						ArrayGetValue($arrayCountry,'text_confirmed'),
						ArrayGetValue($arrayCountry,'text_deaths'),
						ArrayGetValue($arrayCountry,'text_mortality')/*,
						ArrayGetValue($arrayCountry,'text_recovered'),
						ArrayGetValue($arrayCountry,'text_active')*/
						);					
					$arrayCellStyle[] = array('left','right','right','right','right'/*,'right','right'*/);
					$arrayHeaders[] = array(false,false,false,false,false/*,false,false*/);
				}
				
				
				break;
			case 'percapita':
			default:
				$strTab = 'percapita';

				$arrayTable[] = array(
					$strLinkSortByCountry,
					$strLinkSortByPopulation,
					$strLinkSortByConfirmedPerCapita,
					$strLinkSortByDeathsPerCapita,
					$strLinkSortByMortality/*,
					$strLinkSortByRecoveredPerCapita,
					$strLinkSortByActivePerCapita*/);
				$arrayCellStyle[] = array('left','left','left','left','left'/*,'left','left'*/);  
				$arrayHeaders[] = array(true,true,true,true,true/*,true,true*/);

				foreach ($arrayCountries as $arrayCountry)
				{
					$strName = ArrayGetValue($arrayCountry,'name');
					$strID = ArrayGetValue($arrayCountry,'id');
					if (IsValidCountryID($strID))
					{
						$strName = $flag->RenderFlag($strID) . $strName;
					}
		
					$arrayParams = array();
					$arrayParams[$this->GetModuleVarNameState()] = 'country';
					$arrayParams[$this->GetModuleVarName('country')] = $strID;
					$strCountryURL = $this->GetWebsite()->MakeURL($arrayParams);
					$strName = $link->RenderLink($strCountryURL,$strName);
		
					$arrayTable[] = array(
						$strName,
						ArrayGetValue($arrayCountry,'text_population'),
						ArrayGetValue($arrayCountry,'text_confirmedpercapita'),
						ArrayGetValue($arrayCountry,'text_deathspercapita'),
						ArrayGetValue($arrayCountry,'text_mortality')/*,
						ArrayGetValue($arrayCountry,'text_recoveredpercapita'),
						ArrayGetValue($arrayCountry,'text_activepercapita')*/
						);					
					$arrayCellStyle[] = array('left','right','right','right','right'/*,'right','right'*/);
					$arrayHeaders[] = array(false,false,false,false,false/*,false,false*/);
				}


				break;			
			}			

			$tabnavigationDef = new CTabNavigationDefinition();
   
		  $arrayParams = array();
   		$arrayParams[$this->GetModuleVarName('countrylist_tab')] = 'percapita';
		  $tabnavigationDef->AddTab(
      	'percapita',
      	'?TID_LABEL_PERCAPITA?',
      	$this->GetWebsite()->MakeURL($arrayParams));

		  $arrayParams = array();
   		$arrayParams[$this->GetModuleVarName('countrylist_tab')] = 'absolute';
		  $tabnavigationDef->AddTab(
      	'absolute',
      	'?TID_LABEL_ABSOLUTE?',
      	$this->GetWebsite()->MakeURL($arrayParams));


			$this->SetOutputItem(
				'TAB',
				$tabnavigation->RenderTabNavigation($tabnavigationDef,$strTab));

			
			$this->SetOutputItem(
				'TABLE',
				$table->RenderTableEx($arrayTable,$arrayHeaders,$arrayCellStyle));			
			
			return 'countrylist';
		}
		
		function StateCountry()
		{
			$strID = $this->GetModuleVar('country');
			if (!IsValidCountryID($strID))
			{
				$this->SetModuleVar('country','');
				return $this->StateCountryList();
			}
			
			$link = new CLinkElement($this->GetWebsite(),'default');
			$arrayParams = array();
			$arrayParams[$this->GetModuleVarNameState()] = 'countrylist';
			$arrayParams[$this->GetModuleVarName('country')] = '';
			$strCountryListURL = $this->GetWebsite()->MakeURL($arrayParams);
			$this->SetOutputItem('COUNTRYLIST_LINK',$link->RenderLink($strCountryListURL,'?TID_LINK_COUNTRYLIST?'));
	
	
			$arrayOutbreakData = $this->GetOutbreakData();
			$arrayCountry = null;
			foreach (ArrayGetValue($arrayOutbreakData,'countries') as $arrayCountryInArray)
			{
				if (ArrayGetValue($arrayCountryInArray,'id') == $strID)
				{
					$arrayCountry = $arrayCountryInArray;
					break;
				}	
			}
			
			if ($arrayCountry == null)
			{
				return $this->StateCountryList();
			}
			
			
			$this->SetOutputItem('COUNTRY_NAME',ArrayGetValue($arrayCountry,'name'));

			
			$arrayOutbreakData['map'] = array();
			$arrayOutbreakData['map']['lat'] = ArrayGetValue($arrayCountry,'lat');
			$arrayOutbreakData['map']['long'] = ArrayGetValue($arrayCountry,'long');
			$nPopulation = ArrayGetValue($arrayCountry,'population');
			$nZoom = null;
			if ($nPopulation > 5 * 100000000)
			{
				$nZoom = 3;	
			}
			else if ($nPopulation > 100000000)
			{
				$nZoom = 3;	
			}
			else if ($nPopulation > 50000000)
			{
				$nZoom = 5;	
			}
			else if ($nPopulation > 100000)
			{
				$nZoom = 6;	
			}
			else
			{
				$nZoom = 7;	
			}
			$arrayOutbreakData['map']['zoom'] = $nZoom;
			
			$arrayOutbreakData = ArrayRemoveKeyRecursive($arrayOutbreakData,'timeseries');
			
			$this->IncludeElement('outbreakmap');
			$outbreakmap = new COutbreakMapElement($this->GetWebsite(),'country'); 
			$this->SetOutputItem('COUNTRY_MAP',$outbreakmap->RenderOutbreakMap($arrayOutbreakData));


			// Chart with 7 day incidence rates
			
			$arrayLineChartConfigLines = array(
					/*
					array(
						'key' => 'incidence_daily',
						'label' => '?TID_LABEL_INCIDENCE-DAILY?',
						'color' => 'grey',
						'weight' => 1),*/
					array(
						'key' => 'incidence_7days',  
						'label' => '?TID_LABEL_INCIDENCE-7DAYS?',
						'color' => 'blue',
						'weight' => 2  
						),
						
					);
		
			$arrayLineChartConfig = array(
				'lines' => $arrayLineChartConfigLines, 
				'legend' => 'date'
				);
			
			//$this->Trace($arrayCountry);
			
			$arrayTimeSeries = ArrayGetValue($arrayCountry,'timeseries');
			
			if (IsArray($arrayTimeSeries))
			{
				$nLength = ArrayCount($arrayTimeSeries);
				for ($nIndex = 0; $nIndex < $nLength; $nIndex++)
				{
					if ($nIndex != 0 && $nIndex != $nLength-1) 
					{
						$arrayTimeSeries[$nIndex]['date'] = '';	
					}
				}
				$this->IncludeElement('linechart');
				$linechart = new CLineChartElement($this->GetWebsite(),'country'); 
				$this->SetOutputItem(
					'COUNTRY_CHART_INCIDENCE',
					$linechart->RenderLineChart(
						$arrayTimeSeries,
						$arrayLineChartConfig));
						
				$this->IncludeElement('legend');
				$legend = new CLegendElement($this->GetWebsite(),'default'); 
						
				$this->SetOutputItem(
					'COUNTRY_CHART_INCIDENCE_LEGEND', 
					$legend->RenderLegend(
						$arrayLineChartConfigLines));
			}
			else
			{
				$this->SetOutputItem('COUNTRY_CHART_INCIDENCE','');
				$this->SetOutputItem('COUNTRY_CHART_INCIDENCE_LEGEND','');
			}


			// Chart with daily delta data
			
			$arrayLineChartConfigLines = array(
					array(
						'key' => 'confirmed_delta_daily',
						'label' => '?TID_LABEL_CONFIRMED-DELTA-DAILY?',
						'color' => 'grey',
						'weight' => 1),
					array(
						'key' => 'confirmed_delta_average7d',  
						'label' => '?TID_LABEL_CONFIRMED-DELTA-AVERAGE7D?',
						'color' => 'blue',
						'weight' => 2  
						),
						
					);
		
			$arrayLineChartConfig = array(
				'lines' => $arrayLineChartConfigLines, 
				'legend' => 'date'
				);
			
			
			$arrayTimeSeries = ArrayGetValue($arrayCountry,'timeseries');
			if (IsArray($arrayTimeSeries))
			{
				$nLength = ArrayCount($arrayTimeSeries);
				for ($nIndex = 0; $nIndex < $nLength; $nIndex++)
				{
					if ($nIndex != 0 && $nIndex != $nLength-1) 
					{
						$arrayTimeSeries[$nIndex]['date'] = '';	
					}
				}
				$this->IncludeElement('linechart');
				$linechart = new CLineChartElement($this->GetWebsite(),'country'); 
				$this->SetOutputItem(
					'COUNTRY_CHART_DELTA',
					$linechart->RenderLineChart(
						$arrayTimeSeries,
						$arrayLineChartConfig));
						
				$this->IncludeElement('legend');
				$legend = new CLegendElement($this->GetWebsite(),'default'); 
						
				$this->SetOutputItem(
					'COUNTRY_CHART_DELTA_LEGEND', 
					$legend->RenderLegend(
						$arrayLineChartConfigLines));
			}
			else
			{
				$this->SetOutputItem('COUNTRY_CHART_DELTA','');
				$this->SetOutputItem('COUNTRY_CHART_DELTA_LEGEND','');
			}







			// Chart with absolute data
			
			$arrayLineChartConfigLines = array(
					array(
						'key' => 'confirmed',
						'label' => '?TID_LABEL_CONFIRMED?',
						'color' => 'blue'),
					array(
						'key' => 'deaths',
						'label' => '?TID_LABEL_DEATHS?',
						'color' => 'red'
						)/*,
					array(
						'key' => 'recovered',
						'label' => '?TID_LABEL_RECOVERED?',
						'color' => 'green'
						),
					array(
						'key' => 'active',
						'label' => '?TID_LABEL_ACTIVE?',
						'color' => 'orange'
						)*/
					);
		
			$arrayLineChartConfig = array(
				'lines' => $arrayLineChartConfigLines,
				'legend' => 'date'
				);
			
			
			$arrayTimeSeries = ArrayGetValue($arrayCountry,'timeseries');
			if (IsArray($arrayTimeSeries))
			{
				$nLength = ArrayCount($arrayTimeSeries);
				for ($nIndex = 0; $nIndex < $nLength; $nIndex++)
				{
					if ($nIndex != 0 && $nIndex != $nLength-1) 
					{
						$arrayTimeSeries[$nIndex]['date'] = '';	
					}
				}
				$this->IncludeElement('linechart');
				$linechart = new CLineChartElement($this->GetWebsite(),'country'); 
				$this->SetOutputItem(
					'COUNTRY_CHART_ABSOLUTE',
					$linechart->RenderLineChart(
						$arrayTimeSeries,
						$arrayLineChartConfig));
						
				$this->IncludeElement('legend');
				$legend = new CLegendElement($this->GetWebsite(),'default'); 
						
				$this->SetOutputItem(
					'COUNTRY_CHART_ABSOLUTE_LEGEND', 
					$legend->RenderLegend(
						$arrayLineChartConfigLines));
			}
			else
			{
				$this->SetOutputItem('COUNTRY_CHART_ABSOLUTE','');
				$this->SetOutputItem('COUNTRY_CHART_ABSOLUTE_LEGEND','');
			}
		
		
			$table = new CTableElement($this->GetWebsite(),'default');
			
			$arrayTable = array();
			$arrayHeaders = array();
			$arrayCellStyle = array();
			
			$arrayTable[] = array('','?TID_LABEL_ABSOLUTE?','?TID_LABEL_PERCAPITA?');
			$arrayCellStyle[] = array('left','left','left');
			$arrayHeaders[] = array(false,true,true);

			function AddTableRow($strLabelTID,$strKeyAbsolute,$strKeyPerCapita,$arrayCountry,&$arrayTable,&$arrayCellStyle,&$arrayHeaders)
			{
				$arrayTable[] = array(
					'?TID_LABEL_'.$strLabelTID.'?',
					ArrayGetValue($arrayCountry,$strKeyAbsolute),
					ArrayGetValue($arrayCountry,$strKeyPerCapita));
				$arrayCellStyle[] = array('left','right','right');
				$arrayHeaders[] = array(true,false,false);		
			}
			AddTableRow('POPULATION','text_population','',$arrayCountry,$arrayTable,$arrayCellStyle,$arrayHeaders);
			AddTableRow('CONFIRMED','text_confirmed','text_confirmedpercapita',$arrayCountry,$arrayTable,$arrayCellStyle,$arrayHeaders);
			AddTableRow('DEATHS','text_deaths','text_deathspercapita',$arrayCountry,$arrayTable,$arrayCellStyle,$arrayHeaders);
			AddTableRow('MORTALITY','text_mortality','',$arrayCountry,$arrayTable,$arrayCellStyle,$arrayHeaders);
			//AddTableRow('RECOVERED','text_recovered','text_recoveredpercapita',$arrayCountry,$arrayTable,$arrayCellStyle,$arrayHeaders);
			//AddTableRow('ACTIVE','text_active','text_activepercapita',$arrayCountry,$arrayTable,$arrayCellStyle,$arrayHeaders);
						
			$this->SetOutputItem(
				'COUNTRY_TABLE',
				$table->RenderTableEx($arrayTable,$arrayHeaders,$arrayCellStyle));			

						
			return 'country';
		}

	}

