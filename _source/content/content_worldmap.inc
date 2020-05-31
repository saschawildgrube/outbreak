<?php

	class CMyContent extends CContent
	{
		function CallbackRenderContent(&$arrayItems)
		{
			$this->GetWebsite()->m_moduleCovid19->SetState('worldmap');
			$this->SetOutputItem('OUTBREAK_WORLDMAP',$this->GetWebsite()->m_moduleCovid19->RenderModule());
		}
	}
	
