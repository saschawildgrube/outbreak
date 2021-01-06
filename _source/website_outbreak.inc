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
			$arrayConfig['session_mode'] = SESSION_MODE_LOCAL_COOKIE;
			$arrayConfig['cache_deactivate'] = true;
			$arrayConfig['cache_allow_nocache'] = true;
			
			$arrayConfig['check_unresolvedtid'] = true;
			$arrayConfig['check_invalidtid'] = true;
			$arrayConfig['check_unintendedoutput'] = true;
			
			$arrayConfig['languages'][] = 'EN'; 
			$arrayConfig['defaultcountry'] = 'USA';
			$arrayConfig['countries'][] = 'USA';
			
			$arrayConfig['opengraph'] = true;
			$arrayConfig['layouts'][] = 'outbreak';
			$arrayConfig['protocols'][] = 'https';
			$arrayConfig['admin_email'] = GetAdminEmail();
			$arrayConfig['seourl'] = 'seourl';
			$arrayConfig['csvformat'] = 'excel';
			
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
		
		function OnInit()
		{
			$bResult = parent::OnInit();
			
			$this->SetLayout('outbreak');
		
			$strContent = $this->GetContent();
			if ($strContent == '' || $strContent == 'index')
			{
				$this->SetContent('worldmap');	
			}

			$this->SetTimezone('Europe/Berlin');
			
			
			// MONITORING
			$this->AddHeader('<!--AVAILABILITY-CHECK-->');			

			// FORK ME ON GITHUB
			$this->IncludeElement('forkmeongithub');
			$forkmeongithub = new CForkMeOnGithubElement($this,'default');
			$this->SetOutputItem('FORKMEONGITHUB',$forkmeongithub->RenderForkMeOnGithub('saschawildgrube','outbreak'));

			// THE ACTUAL OUTBREAK MODULE
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
			return '%Y-%m-%d';
		}

		function CallbackGetTimeLocalFormat($strCountryID)
		{
			return '%H:%M:%S %Z'; 
		}
		
		function CallbackPrepareMakeURL($strRootURL,&$arrayParams,$bEncoded)
		{
			$this->Trace('COutbreakWebSite::CallbackPrepareMakeURL');
			$this->Trace($arrayParams);

			if ($this->m_moduleCovid19 != null)
			{
				$strModuleState = ArrayGetValue($arrayParams,$this->m_moduleCovid19->GetModuleVarNameState());
				if ($strModuleState == 'country')
				{
					$strCountryID = StringUpperCase(ArrayGetValue($arrayParams,$this->m_moduleCovid19->GetModuleVarName('country')));
					if (IsValidCountryID($strCountryID))
					{
						$strURL = $strRootURL . StringLowerCase($strCountryID);
						$arrayParams= ArrayRemoveKey($arrayParams,$this->m_moduleCovid19->GetModuleVarNameState());
						$arrayParams= ArrayRemoveKey($arrayParams,$this->m_moduleCovid19->GetModuleVarName('country'));
						$arrayParams= ArrayRemoveKey($arrayParams,'content');
						$this->Trace($strURL);
						return $strURL;
					}
				}
			}
			return parent::CallbackPrepareMakeURL($strRootURL,$arrayParams,$bEncoded);
		}
		
		function CallbackParseSEOURL($strSEOURL)
		{
			$this->Trace('COutbreakWebSite::CallbackParseSEOURL: '.$strSEOURL);
			$strCountryID = StringUpperCase($strSEOURL);
			if (IsValidCountryID($strCountryID))
			{
				//$this->Trace('Country ID from seourl detected: '.$strCountryID);
				$this->SetContent('countries');
				$this->SetVar('covid19_state','country');
				$this->SetVar('covid19_country',$strCountryID);
			}
			else
			{
				parent::CallbackParseSEOURL($strSEOURL);
			}
			
		}

		
		
	
	}
	

