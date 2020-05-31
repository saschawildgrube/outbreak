<?php

	class CMyContent extends CContent
	{
		function CallbackRenderContent(&$arrayItems)
		{
			$this->GetWebsite()->IncludeModule('outbreak/covid19');
			$arrayConfig = array();
			$covid19 = new CCOVID19Module($this->GetWebsite(),'',$arrayConfig);
			$arrayItems['MODULE_COVID19'] = $covid19->RenderModule();
		}
	}
	
