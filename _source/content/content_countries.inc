<?php


	class CMyContent extends CContent
	{
		function OnRenderContent(&$arrayItems)
		{
			$this->GetWebsite()->m_moduleCovid19->SetState('countrylist');
			$this->SetOutputItem('OUTBREAK_COUNTRYLIST',$this->GetWebsite()->m_moduleCovid19->RenderModule());
		}
	}
	
