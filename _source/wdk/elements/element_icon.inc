<?php

	class CIconElement extends CElement
	{
		function __construct(
			CWebSite $website,
			$strStyleID)
		{
			parent::__construct(
				$website,
				'icon',
				$strStyleID,
				true,
				true);
				
			$this->GetWebsite()->ActivateFontAwesome();
		}
		
		function RenderIcon($strIconID)
		{
			$strLayoutDef = $this->GetLayoutHTML();
			
			$arrayItems = array();
			
			$strAliasIconID = $this->GetLayoutDef('LAYOUT_ICON_'.StringUpperCase($strIconID));
			if ($strAliasIconID != '')
			{
				$strIconID = $strAliasIconID;
			}
			
			$arrayItems["ICONID"] = StringLowerCase($strIconID);

/*
			if (FindString($strLayoutDef,"IMAGEURL") != -1)
			{
				$arrayItems["IMAGEURL"] = HtmlEncode($this->GetWebsite()->GetLayoutImageURL("icon_".$strIconID,"png"));
			}
*/
/*
			if (FindString($strLayoutDef,"TOOLTIP") != -1)
			{
				$arrayItems["TOOLTIP"] = HtmlEncode($strToolTip);
			}
*/			

// <i class="fa fa-{ICONID} {SPINNER} fa-fw"></i>


			$this->SetOutputItems($arrayItems);
			$strOutput = $this->ReplaceOutputItems($strLayoutDef);
			return $strOutput;			
		}
	}
	
	
	function GetIconIDByFileCategory($strFileCategory)
	{
		if ($strFileCategory == 'other')
		{
			return 'file-o';	
		}
		return 'file-'.$strFileCategory.'-o';
	}

