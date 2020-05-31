<?php


	class CMyContent extends CContent
	{
		function CallbackRenderContent(&$arrayItems)
		{
			$this->GetWebsite()->m_moduleCovid19->SetState('countrylist');
			$this->SetOutputItem('OUTBREAK_COUNTRYLIST',$this->GetWebsite()->m_moduleCovid19->RenderModule());
		}
	}
	
