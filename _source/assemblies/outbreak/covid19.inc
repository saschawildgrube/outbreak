<?php



		// https://developers.google.com/public-data/docs/canonical/countries_csv
		
		// http://www.geonames.org/export/web-services.html
		// https://www.who.int/data/gho/info/gho-odata-api
		// https://raw.githubusercontent.com/CSSEGISandData/COVID-19/master/csse_covid_19_data/csse_covid_19_daily_reports/03-30-2020.csv
		// https://apify.com/covid-19
		// https://api.apify.com/v2/key-value-stores/OHrZyNo9BzT6xKMRD/records/LATEST?disableRedirect=true
		// https://usafacts.org/visualizations/coronavirus-covid-19-spread-map/





	function ComputeMortality($arrayObject)
	{
		$nConfirmed = GetIntegerValue(ArrayGetValue($arrayObject,'confirmed'));
		$nDeaths = GetIntegerValue(ArrayGetValue($arrayObject,'deaths'));
		$fMortality = 0.0;
		if ($nConfirmed > 0)
		{
			$fMortality =
				round(
					GetFloatValue(
						$nDeaths / $nConfirmed * 100),
					2);
		}
		return $fMortality;
	}

	function GetLatestDataSetFromTimeSeries($arrayObject)
	{
		$arrayTimeSeries = ArrayGetValue($arrayObject,'timeseries');
		if (!is_array($arrayTimeSeries))
		{
			return array();	
		}
		if (ArrayCount($arrayTimeSeries) < 1)
		{
			return array();	
		}
		return array_pop($arrayTimeSeries);
	}
					
	function GetHarvardCountryData()
	{
		$strCountryDataFile = GetAssemblyDir('outbreak').'data/country_centroids_az8.csv';
		$arrayCountries = FileReadCSV(
			$strCountryDataFile,
			true,
			false,
			',',
			array(),
			null,
			null,
			false);
		if (!is_array($arrayCountries))
		{
			$arrayCountries = array();
		}
		return $arrayCountries;
	}


	function GetJohnsHopkinsTimeSeriesFromRow($arrayRow,$strValueName)
	{
		$arrayColumns = array_keys($arrayRow);
		$arrayTimeSeries = array();
		$nLastValue = 0;
		foreach ($arrayColumns as $strColumn)
		{
			if (StringCheckCharSet($strColumn,CHARSET_NUMERIC.'/') == true)
			{
				$arrayDate = StringExplode($strColumn,'/');
				if (ArrayCount($arrayDate) == 3)
				{
					$strYear = str_pad($arrayDate[2],2,'0',STR_PAD_LEFT);
					$strMonth = str_pad($arrayDate[0],2,'0',STR_PAD_LEFT);
					$strDay = str_pad($arrayDate[1],2,'0',STR_PAD_LEFT);
					$strDate = '20'.$strYear.'-'.$strMonth.'-'.$strDay;	
				}
				$arrayTimeSeries[$strDate]['date'] = $strDate;
				// This is because the time series for recovered cases in the UK suddenly continues with all zeros (as of 2020-05-02)
				$nLastValue = max($nLastValue,GetIntegerValue(ArrayGetValue($arrayRow,$strColumn)));
				$arrayTimeSeries[$strDate][$strValueName] = $nLastValue;
			}	
		}
		return $arrayTimeSeries;
		
	}

	function GetJohnsHopkinsOutbreakData()
	{
		$strConfirmedURL =	'https://raw.githubusercontent.com/CSSEGISandData/COVID-19/master/csse_covid_19_data/csse_covid_19_time_series/time_series_covid19_confirmed_global.csv';
		$strDeathsURL = 		'https://raw.githubusercontent.com/CSSEGISandData/COVID-19/master/csse_covid_19_data/csse_covid_19_time_series/time_series_covid19_deaths_global.csv';
		$strRecoveredURL = 	'https://raw.githubusercontent.com/CSSEGISandData/COVID-19/master/csse_covid_19_data/csse_covid_19_time_series/time_series_covid19_recovered_global.csv';

		$strConfirmedRaw = HttpRequest($strConfirmedURL);
		$strDeathsRaw = HttpRequest($strDeathsURL);
		$strRecoveredRaw = HttpRequest($strRecoveredURL);

		$arrayConfirmedRaw = ParseCSV($strConfirmedRaw,true,false,',',array(),null,null,false);
		$arrayDeathsRaw = ParseCSV($strDeathsRaw,true,false,',',array(),null,null,false);
		$arrayRecoveredRaw = ParseCSV($strRecoveredRaw,true,false,',',array(),null,null,false);

		$arrayCountries = array();
		$arrayShips = array();

		// Johns Hopkins confuses dependent countries with provinces		
		$arrayDependencyCountries = array();
		$arrayDependencyCountries[] = 'Netherlands';
		$arrayDependencyCountries[] = 'United Kingdom';
		$arrayDependencyCountries[] = 'France';
		$arrayDependencyCountries[] = 'Denmark';
		
		$arrayDependenciesAsCountries = array();
		// Exception for Hong Kong and Macao
		$arrayDependenciesAsCountries[] = 'Macau';  
		$arrayDependenciesAsCountries[] = 'Hong Kong';

		
		foreach($arrayConfirmedRaw as $arrayRow)
		{
			$arrayTimeSeries = GetJohnsHopkinsTimeSeriesFromRow($arrayRow,'confirmed');
					
			$strCountry = ArrayGetValue($arrayRow,'Country/Region');
			$strProvince = ArrayGetValue($arrayRow,'Province/State');
			$strLat = ArrayGetValue($arrayRow,'Lat');
			$strLong = ArrayGetValue($arrayRow,'Long');				


			if (($strLat == '0' || $strLong == '0') && ($strProvince == '')) // As far as we know these are ships
			{
				$strShip = $strCountry; // Don't ask
				$arrayShip = ArrayGetValue($arrayShips,$strShip);
				if (!is_array($arrayShip))
				{
					$arrayShip = array();
					$arrayShip['name'] = $strShip;
					$arrayShip['timeseries'] = $arrayTimeSeries;
				}
				$arrayShips[$strShip] = $arrayShip;
			}
			else
			{

				if ($strProvince != '')
				{
					if (		ArrayValueExists($arrayDependencyCountries,$strCountry)
							|| 	ArrayValueExists($arrayDependenciesAsCountries,$strProvince))
					{
						$strCountry = $strProvince;
						$strProvince = '';	
					}	
				}
	
				
				$arrayCountry = ArrayGetValue($arrayCountries,$strCountry);
				if (!is_array($arrayCountry))
				{
					$arrayCountry = array();
					$arrayCountry['name'] = $strCountry;
				}
				
				if ($strProvince != '')
				{
					$arrayProvince = ArrayGetValue($arrayCountry,'provinces',$strProvince);
					if (!is_array($arrayProvince))
					{
						$arrayProvince = array();
						$arrayProvince['name'] = $strProvince;
					}
					
					$arrayProvince['lat'] = $strLat;
					$arrayProvince['long'] = $strLong;				
					$arrayProvince['timeseries'] = $arrayTimeSeries;
					
					$arrayCountry['provinces'][$strProvince] = $arrayProvince;
				}
				else
				{
					$arrayCountry['lat'] = $strLat;
					$arrayCountry['long'] = $strLong;				
					$arrayCountry['timeseries'] = $arrayTimeSeries;
				}
				$arrayCountries[$strCountry] = $arrayCountry;
			}
		}
	
	
		foreach($arrayDeathsRaw as $arrayRow)
		{
			$arrayTimeSeries = GetJohnsHopkinsTimeSeriesFromRow($arrayRow,'deaths');

					
			$strCountry = ArrayGetValue($arrayRow,'Country/Region');
			$strProvince = ArrayGetValue($arrayRow,'Province/State');
			$strLat = ArrayGetValue($arrayRow,'Lat');
			$strLong = ArrayGetValue($arrayRow,'Long');				

			if (($strLat == '0' || $strLong == '0') && ($strProvince == '')) // As far as we know these are ships
			{
				$strShip = $strCountry; // Don't ask
				$arrayShip = ArrayGetValue($arrayShips,$strShip);
				if (!is_array($arrayShip))
				{
					$arrayShip = array();
					$arrayShip['name'] = $strShip;
				}
				
				$arrayShip['timeseries'] = ArrayMergeRecursive($arrayShip['timeseries'],$arrayTimeSeries);
		
				$arrayShips[$strShip] = $arrayShip;
			}
			else
			{
				if ($strProvince != '')
				{
					if (		ArrayValueExists($arrayDependencyCountries,$strCountry)
							|| 	ArrayValueExists($arrayDependenciesAsCountries,$strProvince))
					{
						$strCountry = $strProvince;
						$strProvince = '';	
					}	
				}
	
				
				$arrayCountry = ArrayGetValue($arrayCountries,$strCountry);
				if (!is_array($arrayCountry))
				{
					$arrayCountry = array();
					$arrayCountry['name'] = $strCountry;
				}
				
				if ($strProvince != '')
				{
					$arrayProvince = ArrayGetValue($arrayCountry,'provinces',$strProvince);
					if (!is_array($arrayProvince))
					{
						$arrayProvince = array();
						$arrayProvince['name'] = $strProvince;
					}
					
					$arrayProvince['lat'] = $strLat;
					$arrayProvince['long'] = $strLong;				
					$arrayProvince['timeseries'] = ArrayMergeRecursive($arrayProvince['timeseries'],$arrayTimeSeries);
					
					$arrayCountry['provinces'][$strProvince] = $arrayProvince;
				}
				else
				{
					$arrayCountry['lat'] = $strLat;
					$arrayCountry['long'] = $strLong;				
					$arrayCountry['timeseries'] = ArrayMergeRecursive($arrayCountry['timeseries'],$arrayTimeSeries);

				}
				$arrayCountries[$strCountry] = $arrayCountry;
			}
		}
		

		foreach($arrayRecoveredRaw as $arrayRow)
		{
			$arrayTimeSeries = GetJohnsHopkinsTimeSeriesFromRow($arrayRow,'recovered');

					
			$strCountry = ArrayGetValue($arrayRow,'Country/Region');
			$strProvince = ArrayGetValue($arrayRow,'Province/State');
			$strLat = ArrayGetValue($arrayRow,'Lat');
			$strLong = ArrayGetValue($arrayRow,'Long');				

			if (($strLat == '0' || $strLong == '0') && ($strProvince == '')) // As far as we know these are ships
			{
				$strShip = $strCountry; // Don't ask
				$arrayShip = ArrayGetValue($arrayShips,$strShip);
				if (!is_array($arrayShip))
				{
					$arrayShip = array();
					$arrayShip['name'] = $strShip;
				}
				
				$arrayShip['timeseries'] = ArrayMergeRecursive($arrayShip['timeseries'],$arrayTimeSeries);
		
				$arrayShips[$strShip] = $arrayShip;
			}
			else
			{
				if ($strProvince != '')
				{
					if (		ArrayValueExists($arrayDependencyCountries,$strCountry)
							|| 	ArrayValueExists($arrayDependenciesAsCountries,$strProvince))
					{
						$strCountry = $strProvince;
						$strProvince = '';	
					}	
				}
	
				
				$arrayCountry = ArrayGetValue($arrayCountries,$strCountry);
				if (!is_array($arrayCountry))
				{
					$arrayCountry = array();
					$arrayCountry['name'] = $strCountry;
				}
				
				if ($strProvince != '')
				{
					$arrayProvince = ArrayGetValue($arrayCountry,'provinces',$strProvince);
					if (!is_array($arrayProvince))
					{
						$arrayProvince = array();
						$arrayProvince['name'] = $strProvince;
					}
					
					$arrayProvince['lat'] = $strLat;
					$arrayProvince['long'] = $strLong;				
					$arrayProvince['timeseries'] = ArrayMergeRecursive(ArrayGetValue($arrayProvince,'timeseries'),$arrayTimeSeries);
					
					$arrayCountry['provinces'][$strProvince] = $arrayProvince;
				}
				else
				{
					$arrayCountry['lat'] = $strLat;
					$arrayCountry['long'] = $strLong;				
					$arrayCountry['timeseries'] = ArrayMergeRecursive(ArrayGetValue($arrayCountry,'timeseries'),$arrayTimeSeries);
				}
				$arrayCountries[$strCountry] = $arrayCountry;
			}
		}		

			
		$arrayOutbreakData = array();
		$arrayOutbreakData['countries'] = $arrayCountries;
		$arrayOutbreakData['ships'] = $arrayShips;
		return $arrayOutbreakData;
	}


	function GetOutbreakDataCovid19()
	{
		$arrayOutbreakData = array();
		$arrayOutbreakData['sources'] = array();
		$arrayOutbreakData['world'] = array();
		$arrayOutbreakData['countries'] = array();
		$arrayOutbreakData['ships'] = array();
		
		$strLatestDataDate = '';
			

		$arrayWorld = array();	
		$arrayCountries = array();
		$arrayShips = array();

		// Prepare Countries
		$arrayCountryIDs = GetAllCountryIDs();
		foreach ($arrayCountryIDs as $strCountryID)
		{
			$arrayCountries[$strCountryID] = array(
				'id' => $strCountryID
				);	
		}
		
		
		// Harvard
		$arrayHarvardCountries = GetHarvardCountryData();


		// Prepare name to id mapping based on harvard country data
		$arrayNameToID = array();
		$arrayLongNameToID = array();
		$arrayIDToName = array();
		$arrayHarvardCountriesByID = array();
		foreach ($arrayHarvardCountries as $arrayHarvardCountry)
		{
			$strID = ArrayGetValue($arrayHarvardCountry,'iso_a3');
			$arrayNameToID[ArrayGetValue($arrayHarvardCountry,'name')] = $strID;
			$arrayLongNameToID[ArrayGetValue($arrayHarvardCountry,'name_long')] = $strID;
			$arrayHarvardCountriesByID[$strID] = $arrayHarvardCountry;
		}
		
		// Name mapping for Johns Hopkins
		$arrayJohnsHopkinsNameToID['US'] = 'USA';
		$arrayJohnsHopkinsNameToID['Korea, South'] = 'KOR';
		$arrayJohnsHopkinsNameToID['Czechia'] = 'CZE';
		$arrayJohnsHopkinsNameToID['North Macedonia'] = 'MKD';
		$arrayJohnsHopkinsNameToID['Taiwan*'] = 'TWN';
		$arrayJohnsHopkinsNameToID["Cote d'Ivoire"] = 'CIV';
		$arrayJohnsHopkinsNameToID['West Bank and Gaza'] = 'PAL';
		$arrayJohnsHopkinsNameToID['Holy See'] = 'VAT';
		$arrayJohnsHopkinsNameToID['Cabo Verde'] = 'CPV'; 
		$arrayJohnsHopkinsNameToID['Burma'] = 'MMR';  
		$arrayJohnsHopkinsNameToID['Congo (Brazzaville)'] = 'COG';  
		$arrayJohnsHopkinsNameToID['Congo (Kinshasa)'] = 'COD';  
		$arrayJohnsHopkinsNameToID['Eswatini'] = 'SWZ';
		$arrayJohnsHopkinsNameToID['Laos'] = 'LAO';
		$arrayJohnsHopkinsNameToID['Kosovo'] = 'XKX';
		$arrayJohnsHopkinsNameToID['Sao Tome and Principe'] = 'STP';
		$arrayJohnsHopkinsNameToID['Macau'] = 'MAC';
		$arrayJohnsHopkinsNameToID['Saint Barthelemy'] = 'BLM';
		$arrayJohnsHopkinsNameToID['Bonaire, Sint Eustatius and Saba'] = 'BES';
		$arrayJohnsHopkinsNameToID['Faroe Islands'] = 'FRO';
		$arrayJohnsHopkinsNameToID['Curacao'] = 'CUW';
		$arrayJohnsHopkinsNameToID['Falkland Islands (Malvinas)'] = 'FLK';
		$arrayJohnsHopkinsNameToID['French Guiana'] = 'GUF';
		$arrayJohnsHopkinsNameToID['Gibraltar'] = 'GIB';
		$arrayJohnsHopkinsNameToID['Guadeloupe'] = 'GLP';
		$arrayJohnsHopkinsNameToID['Mayotte'] = 'MYT';
		$arrayJohnsHopkinsNameToID['Reunion'] = 'REU';
		$arrayJohnsHopkinsNameToID['St Martin'] = 'MAF';
		$arrayJohnsHopkinsNameToID['Martinique'] = 'MTQ';
		$arrayJohnsHopkinsNameToID['West Bank and Gaza'] = 'PSE';

		// Johns Hopkins
		$arrayJohnsHopkinsOutbreakData = GetJohnsHopkinsOutbreakData();
		
		// Process Johns Hopkins countries
		$arrayJohnsHopkinsCountries = ArrayGetValue($arrayJohnsHopkinsOutbreakData,'countries');
		foreach ($arrayJohnsHopkinsCountries as &$arrayJohnsHopkinsCountry)
		{
			$strCountry = ArrayGetValue($arrayJohnsHopkinsCountry,'name');
			$strID = ArrayGetValue($arrayNameToID,$strCountry);
			if ($strID == '')
			{
				$strID = ArrayGetValue($arrayLongNameToID,$strCountry);
			}
			if ($strID == '')
			{
				$strID = ArrayGetValue($arrayJohnsHopkinsNameToID,$strCountry);
			}
			if ($strID == '')
			{
				$strID = $strCountry;
			}
			
			$arrayCountry = ArrayGetValue($arrayCountries,$strID);
			if (!is_array($arrayCountry))
			{
				$arrayCountry = array();
				$arrayCountry['name'] = $strCountry.'';
				$arrayCountry['id'] = '';
			}

			$arrayLatestDataSet = GetLatestDataSetFromTimeSeries($arrayJohnsHopkinsCountry);
			$arrayCountry['confirmed'] = GetIntegerValue(ArrayGetValue($arrayLatestDataSet,'confirmed'));
			$arrayCountry['deaths'] = GetIntegerValue(ArrayGetValue($arrayLatestDataSet,'deaths'));
			$arrayCountry['recovered'] = GetIntegerValue(ArrayGetValue($arrayLatestDataSet,'recovered'));
			$arrayCountry['active'] = $arrayCountry['confirmed'] - $arrayCountry['deaths'] - $arrayCountry['recovered'];

			// Determine the latest data date:
			$strDate = ArrayGetValue($arrayLatestDataSet,'date');
			if (CompareString($strDate,$strLatestDataDate) > 0)    
			{
				$strLatestDataDate = $strDate;
			}

			$arrayProvinces = ArrayGetValue($arrayJohnsHopkinsCountry,'provinces');
			if (is_array($arrayProvinces))
			{
				$nCountryConfirmed = $arrayCountry['confirmed'];
				$nCountryDeaths = $arrayCountry['deaths'];
				$nCountryRecovered = $arrayCountry['recovered'];
				$nCountryActive = $arrayCountry['active'];
				foreach ($arrayProvinces as &$arrayProvince)
				{
					$arrayLatestDataSet = GetLatestDataSetFromTimeSeries($arrayProvince);
					$nConfirmed = GetIntegerValue(ArrayGetValue($arrayLatestDataSet,'confirmed'));
					$nDeaths = GetIntegerValue(ArrayGetValue($arrayLatestDataSet,'deaths'));
					$nRecovered = GetIntegerValue(ArrayGetValue($arrayLatestDataSet,'recovered'));
					$nActive = $nConfirmed - $nDeaths - $nRecovered;
					$arrayProvince['active'] = $nActive;
				
					$nCountryConfirmed += $nConfirmed;
					$nCountryDeaths += $nDeaths;
					$nCountryRecovered += $nRecovered;
					$nCountryActive += $nActive;
										
					$arrayCountry['timeseries'] = ArrayMergeRecursive(
						ArrayGetValue($arrayCountry,'timeseries'),
						ArrayGetValue($arrayProvince,'timeseries'),
						'CallbackArrayMergeAdd');
				}
				$arrayCountry['confirmed'] = $nCountryConfirmed;
				$arrayCountry['deaths'] = $nCountryDeaths;
				$arrayCountry['recovered'] = $nCountryRecovered;
				$arrayCountry['active'] = $nCountryActive;
				$arrayCountry['provinces'] = $arrayProvinces;
			}
			
			$arrayCountry['timeseries'] = ArrayMergeRecursive(
				ArrayGetValue($arrayCountry,'timeseries'),
				ArrayGetValue($arrayJohnsHopkinsCountry,'timeseries'),
				'CallbackArrayMergeAdd');

			$arrayCountry['timeseries'] = ArrayValues(ArrayGetValue($arrayCountry,'timeseries'));
			$arrayCountry['mortality'] = ComputeMortality($arrayCountry);
			$arrayCountries[$strID] = $arrayCountry;
		}

		// Process Harvard data and do per capita calculations
		foreach ($arrayCountries as $strID => &$arrayCountry)
		{
			$nPopulation = max(0,GetIntegerValue(ArrayGetValue($arrayHarvardCountriesByID,$strID,'pop_est'))); 
			$nConfirmed = GetIntegerValue(ArrayGetValue($arrayCountry,'confirmed'));  
			$nDeaths = GetIntegerValue(ArrayGetValue($arrayCountry,'deaths'));
			$nRecovered = GetIntegerValue(ArrayGetValue($arrayCountry,'recovered'));
			$nActive = GetIntegerValue(ArrayGetValue($arrayCountry,'active'));
						
			if ($nPopulation > 0)
			{
				$fConfirmedPerCapita = 100000 * $nConfirmed / $nPopulation;
				$fDeathsPerCapita = 100000 * $nDeaths / $nPopulation;
				$fRecoveredPerCapita = 100000 * $nRecovered / $nPopulation;
				$fActivePerCapita = 100000 * $nActive / $nPopulation;
			}
			else
			{
				$fConfirmedPerCapita = 0.0;
				$fDeathsPerCapita = 0.0;
				$fRecoveredPerCapita = 0.0;
				$fActivePerCapita = 0.0;
			}

			$arrayCountry['population'] = $nPopulation;
			$arrayCountry['confirmedpercapita'] = $fConfirmedPerCapita;
			$arrayCountry['deathspercapita'] = $fDeathsPerCapita;
			$arrayCountry['recoveredpercapita'] = $fRecoveredPerCapita;
			$arrayCountry['activepercapita'] = $fActivePerCapita;


			$arrayWorld['population'] = GetIntegerValue(ArrayGetValue($arrayWorld,'population')) + $nPopulation;
			$arrayWorld['confirmed'] = GetIntegerValue(ArrayGetValue($arrayWorld,'confirmed')) + $nConfirmed;
			$arrayWorld['deaths'] = GetIntegerValue(ArrayGetValue($arrayWorld,'deaths')) + $nDeaths;
			$arrayWorld['recovered'] = GetIntegerValue(ArrayGetValue($arrayWorld,'recovered')) + $nRecovered;
			$arrayWorld['active'] = GetIntegerValue(ArrayGetValue($arrayWorld,'active')) + $nActive;

		}

		if (ArrayGetValue($arrayWorld,'confirmed') > 0)
		{	
			$arrayWorld['mortality'] = round(GetFloatValue(ArrayGetValue($arrayWorld,'deaths') / ArrayGetValue($arrayWorld,'confirmed') * 100),2);

/*
			if ($nPopulation > 0)
			{
				$fConfirmedPerCapita = 100000 * $nConfirmed / $nPopulation;
				$fDeathsPerCapita = 100000 * $nDeaths / $nPopulation;
			}
			else
			{
				$fConfirmedPerCapita = 0.0;
				$fDeathsPerCapita = 0.0;
			}
*/

		}
		else
		{
			$arrayWorld['mortality'] = 0.0;
		}


		$arrayCountries = ArrayValues($arrayCountries);	
		$arrayOutbreakData['countries'] = $arrayCountries;

		$arrayOutbreakData['world'] = $arrayWorld;




		// Process ships
		$arrayJohnsHopkinsShips = ArrayGetValue($arrayJohnsHopkinsOutbreakData,'ships');
		foreach ($arrayJohnsHopkinsShips as $arrayShip)
		{
			$strName = ArrayGetValue($arrayShip,'name');
			if ($strName == 'MS Zaandam')  
			{
				// The ship MS Zandaam is in the port of Fort Lauderdale
				$arrayShip['lat'] = 26.093931;
				$arrayShip['long'] = -80.112267;
			}
			else if ($strName == 'Diamond Princess')
			{
				// The ship Diomon Princess is in the port of Yokohama
				$arrayShip['lat'] = 35.443756;
				$arrayShip['long'] = 139.704134;
			}
			else
			{
				// Any other yet unknown ships are at 0 0 (South Atlantic)
				$arrayShip['lat'] = 0.0;
				$arrayShip['long'] = 0.0;
			}
			
			$arrayLatestDataSet = GetLatestDataSetFromTimeSeries($arrayShip);
			
			$arrayShip['confirmed'] = GetIntegerValue(ArrayGetValue($arrayLatestDataSet,'confirmed'));
			$arrayShip['deaths'] = GetIntegerValue(ArrayGetValue($arrayLatestDataSet,'deaths'));
			//$arrayShip['recovered'] = GetIntegerValue(ArrayGetValue($arrayLatestDataSet,'recovered'));
			$arrayShip['mortality'] = ComputeMortality($arrayShip);
			$arrayTimeSeries = ArrayGetValue($arrayShip,'timeseries');
			if (is_array($arrayTimeSeries))
			{
				$arrayShip['timeseries'] = ArrayValues($arrayTimeSeries);
			}
			
			$arrayShips[] = $arrayShip;
		}
		$arrayOutbreakData['ships'] = $arrayShips;		

							

		
		
		// Use centroids from Harvard
		$arrayCountries = $arrayOutbreakData['countries'];
		foreach ($arrayCountries as &$arrayCountry)
		{
			$strID = ArrayGetValue($arrayCountry,'id');
			
			// Some centroids are just.... wrong
			if ($strID == 'FRA')
			{
				$arrayCountry['long'] = 2.2137;
				$arrayCountry['lat'] = 46.2276;
			}
			else if ($strID == 'KIR')
			{
				$arrayCountry['long'] = -157.379322;
				$arrayCountry['lat'] = 1.806662;
			}
			// or missing
			else if ($strID == 'GUF')
			{
				$arrayCountry['long'] = -53.1258;
				$arrayCountry['lat'] = 3.9339;
			}
			else
			{
				$arrayHarvardCountry = ArrayGetValue($arrayHarvardCountriesByID,$strID);
				if (is_array($arrayHarvardCountry))
				{
					$strLong = ArrayGetValue($arrayHarvardCountriesByID,$strID,'Longitude');
					$strLat = ArrayGetValue($arrayHarvardCountriesByID,$strID,'Latitude');
					if ($strLong != null && $strLat != null)
					{
						$arrayCountry['long'] = $strLong;
						$arrayCountry['lat'] = $strLat;
					}
				}
			}
		}
		$arrayOutbreakData['countries'] = $arrayCountries;



		function CallbackArrayMapCalculateTimeseries($key,$value,$arrayParams)
		{
			if ($key === 'timeseries')
			{
				if (IsArray($value) == true)
				{
					$nConfirmedPrevious = 0;
					foreach ($value as &$arrayTimePoint)
					{
						$nConfirmed = GetIntegerValue(ArrayGetValue($arrayTimePoint,'confirmed'));
						$nDeaths = GetIntegerValue(ArrayGetValue($arrayTimePoint,'deaths'));
						$nRecovered = GetIntegerValue(ArrayGetValue($arrayTimePoint,'recovered'));
						
						$nActive = $nConfirmed - $nDeaths - $nRecovered;
						$arrayTimePoint['active'] = $nActive;
						
						$arrayTimePoint['confirmed_delta_daily'] = $nConfirmed - $nConfirmedPrevious;
						$nConfirmedPrevious = $nConfirmed;
					}
					return $value;
				}
				return $value;
			}
			if (IsArray($value) == true)
			{
				return ArrayMap(
					$value,
					'CallbackArrayMapCalculateTimeseries',
					$arrayParams);
			}
			return $value;
		}
		
		$arrayOutbreakData = ArrayMap(
			$arrayOutbreakData,
			'CallbackArrayMapCalculateTimeseries',
			array());

		$arrayCountries = ArrayGetValue($arrayOutbreakData,'countries');
		$arrayCountries = $arrayOutbreakData['countries'];
		foreach ($arrayCountries as &$arrayCountry)
		{
			
		}

		

		$arrayOutbreakData['sources'][] = array(
			'name' => 'Johns Hopkins University & Medicine Coronavirus Resource Center');
	
		$arrayOutbreakData['sources'][] = array(
			'name' => 'Harvard University - Center for Geographic Analysis');
			
		$arrayOutbreakData['datetime_downloaded'] = RenderDateTimeNow();
		$arrayOutbreakData['date_latestdata'] = $strLatestDataDate;
		
		return $arrayOutbreakData;
	}
	
