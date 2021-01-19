<?php

	class CMyContent extends CContent
	{
		function OnRenderContent(&$arrayItems)
		{
			$this->GetWebsite()->m_moduleCovid19->SetState('worldmap');
			$this->SetOutputItem('OUTBREAK_WORLDMAP',$this->GetWebsite()->m_moduleCovid19->RenderModule());
		}
	}
	
