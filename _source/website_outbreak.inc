<?php

	require_once('env.inc');
	require_once(GetWDKDir().'wdk_website.inc');
	
	class COutbreakWebSite extends CWebSite
	{
		
		public $m_moduleCovid19;

		function __construct(
			$arrayParams)
		{
			
			$arrayMeta = array();
			$arrayMeta['version'] = '0.1'; 
			$arrayMeta['author'] = 'Sascha Wildgrube';
			$arrayMeta['copyright'] = '?TID_COPYRIGHT?';
			$arrayMeta['copyright_since'] = '2020';
			
			
			$arrayConfig = array();
			
	
			//$arrayConfig['session_mode'] = SESSION_MODE_NONE;
			$arrayConfig['session_mode'] = SESSION_MODE_LOCAL_COOKIE;

			//$arrayConfig['underconstruction'] = true;

			
			$arrayConfig['cache_deactivate'] = true;
			$arrayConfig['cache_allow_nocache'] = true;
			
			$arrayConfig['check_unresolvedtid'] = true;
			$arrayConfig['check_invalidtid'] = true;
			$arrayConfig['check_unintendedoutput'] = true;
			
			$arrayConfig['languages'][] = 'EN'; 
			$arrayConfig['defaultcountry'] = 'USA';
			$arrayConfig['countries'][] = 'USA';
			
			
			$arrayConfig['layouts'][] = 'outbreak';
			$arrayConfig['protocols'][] = 'https';
			$arrayConfig['admin_email'] = GetAdminEmail();
			$arrayConfig['seourl'] = 'seourl';
			$arrayConfig['csvformat'] = 'excel';
			
			$arrayConfig['googlemaps_apikey'] = 'AIzaSyD7TpOpV3vlxkavmlxZztH_3AKVYVKI2oI';
			


			
			parent::__construct(
				GetRootURL(),
				GetSourceDir(),
				GetDocumentRootDir(),
				$arrayMeta,
				$arrayConfig,
				$arrayParams,
				'website_outbreak');
					
		}
		
		function CallbackInitAssemblyLine(&$assemblyLine)
		{
			$assembly = new CAssembly($this,'google');
			$assemblyLine->AddAssembly($assembly);
	
			$assembly = new CAssembly($this,'outbreak');
			$assemblyLine->AddAssembly($assembly);
			
			$assembly = new CAssembly($this,'d3');
			$assemblyLine->AddAssembly($assembly);
	
			parent::CallbackInitAssemblyLine($assemblyLine);
		}
		
		function CallbackInitRoutingMap()
		{
			parent::CallbackInitRoutingMap();
			$this->LoadRoutingMap('default');
		}
		
		function CallbackInit()
		{
			$bResult = parent::CallbackInit();
			
			$this->SetLayout('outbreak');
		
			$strContent = $this->GetContent();
			if ($strContent == '' || $strContent == 'index')
			{
				$this->SetContent('worldmap');	
			}


			// OPEN GRAPH
			$this->AddHeader('<meta property="og:image" content="{IMAGEURL ID="opengraph"}">');
	   	$arrayParams = array();
	   	$arrayParams["content"] = $this->GetContent();
	   	$strPreferredLink = $this->MakeSitemapURL($arrayParams,true);
			$this->AddHeader('<meta property="og:url" content="'.$strPreferredLink.'">');
			$this->AddHeader('<meta property="og:type" content="website">');
			$this->AddHeader('<meta property="og:title" content="'.$this->CallbackGetWebsiteTitle().'">');
			$this->AddHeader('<meta property="og:image:alt" content="'.$this->CallbackGetWebsiteTitle().'">');
			$this->AddHeader('<meta property="og:description" content="'.$this->CallbackGetWebsiteDescription().'">');


/*
			// DISPLAY DATETIME
			$this->IncludeElement('datetime');
			$datetime = new CDateTimeElement($this,'default');
			$this->SetOutputItem('DATETIME',$datetime->RenderDateTime(GetTimeNow()));	

			// LANGUAGE SELECTION
			$this->IncludeModule('user/languageselection');
			$languageselectionModule = new CLanguageSelectionModule($this,'index');
			$this->SetOutputItem('LANGUAGESELECTION',$languageselectionModule->RenderModule('toolbar'));	
*/

			$this->IncludeModule('outbreak/covid19');
			$arrayConfig = array();
			$this->m_moduleCovid19 = new CCOVID19Module($this,'',$arrayConfig);			


			return $bResult;
		}
		
		function GetSiteName()
		{
			return 'Outbreak';
		}
		
		function GetSiteTeamEmail()
		{
			return 'info@'.GetMailDomain();
		}

		function GetSiteTeamName()
		{
			return 'sascha@wildgrube.com';
		}
	
		function CallbackPreContent()
		{
		}

		function CallbackPostContent()
		{
			$this->SetOutputItem('OUTBREAK_SUMMARY',$this->m_moduleCovid19->RenderSummary());
		}
		
		function CallbackRenderContentException()
		{
			return parent::CallbackRenderContentException();
		}
		
		function CallbackRenderSpecialContent($strContentID)
		{
			return parent::CallbackRenderSpecialContent($strContentID); 
		}
		

		function CallbackRenderNavigation($strOutput)
		{
			
			$navigationtree = new CNavigationTree();
			$navigationtree->LoadNavigationTree($this,'');
			$navigationtree->SetCurrentContext($this->GetContent());
			$navigationtree->SetContextParameterName('content');

			if (FindString($strOutput,'{NAVIGATION_HORIZONTALPATH}') != -1)
			{
				$navigation = new CNavigationElement($this,'horizontalpath');
				$this->SetOutputItem('NAVIGATION_HORIZONTALPATH',$navigation->RenderNavigationHorizontalPath($navigationtree));
	    }

/*         
			if (FindString($strOutput, '{NAVIGATION_VERTICALPATH}') != -1)
			{ 
				$navigation = new CNavigationElement($this,'verticalpath');
				$this->SetOutputItem('NAVIGATION_VERTICALPATH',
					$navigation->RenderNavigationVerticalPath($navigationtree,0));
      }
 */        
			if (FindString($strOutput, '{NAVIGATION_CURRENTPATH}') != -1)
			{ 
				$navigation = new CNavigationElement($this,'currentpath');
				$this->SetOutputItem('NAVIGATION_CURRENTPATH',
					$navigation->RenderNavigationCurrentPath($navigationtree));
			}
			
			if (FindString($strOutput, '{NAVIGATION_VERTICALCURRENTPATH}') != -1)
			{ 
				$navigation = new CNavigationElement($this,'verticalcurrentpath');
				$this->SetOutputItem('NAVIGATION_VERTICALCURRENTPATH',
					$navigation->RenderNavigationVerticalCurrentPath($navigationtree));
			}
         
			if (FindString($strOutput, '{NAVIGATION_FOOTER}') != -1)
			{
				$navigationtree_footer = new CNavigationTree();
				$navigationtree_footer->LoadNavigationTree(
					$this,
					'footer');
				$navigationtree_footer->SetCurrentContext($this->GetContent());
				$navigationtree_footer->SetContextParameterName('content');
         		
				$navigation = new CNavigationElement($this,'footer');
				$this->SetOutputItem('NAVIGATION_FOOTER',$navigation->RenderNavigationHorizontalPath($navigationtree_footer));
			}
		}




		function CallbackCheckCondition($strCondition)
		{
			return parent::CallbackCheckCondition($strCondition);	
		}
		
		
		function CallbackGetDateLocalFormat($strCountryID)
		{
			switch ($strCountryID)
			{
				/*
			case 'USA':
				return '%Y-%m-%d';
				*/
			default:
				return '%Y-%m-%d';
			}
		}

		function CallbackGetTimeLocalFormat($strCountryID)
		{
			switch ($strCountryID)
			{
				/*
				case 'USA':				
					return '%H:%M:%S';
				*/
				default:
					return '%H:%M:%S';
			}
		}
	
	}
	

