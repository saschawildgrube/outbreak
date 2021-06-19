// GoogleMapsReactComponent
'use strict';

class GoogleMapsReactComponent extends React.Component
{
  constructor(props)
  {
    super(props);
  	this.m_strId = 'googlemaps_'+GetRandomToken(5);  
  }
  
  componentDidMount()
  {
  	this.RenderGoogleMaps(''+this.m_strId, this.props);
  }

  render()
  {
  	var nHeight = 600;   
  	var nWidth = 800;
  	if (this.props['height'] > 0)
  	{
  		nHeight = this.props['height'];
  	}
   	if (this.props['width'] > 0)
  	{
  		nWidth = this.props['width'];
  	}
  	return e(
  		'div', 
  		{
  			id: this.m_strId,
  			style:
  			{
  				height: nHeight,
  				width: nWidth
  			}
  		},
  		null);
  }
  
	RenderGoogleMaps(strSelector,aProps)
	{
			
		var fLat = GetValue(aProps,'lat');
		if (fLat == null)
		{
			fLat = 30;
		}
		var fLong = GetValue(aProps,'long');
		if (fLong == null)
		{
			fLong = 0;
		}
			
		var nZoom = GetValue(aProps,'zoom');
		if (nZoom == null)
		{
			nZoom = 2;
		}
			
		var strType = GetValue(aProps,'type');
		if (strType == null)
		{
			strType = 'terrain';
		}

		var bTypeControl = GetValue(aProps,'typecontrol');
		if (bTypeControl == null)
		{
			bTypeControl = true;
		}

		var bStreetViewControl = GetValue(aProps,'streetviewcontrol');
		if (bStreetViewControl == null)
		{
			bStreetViewControl = true;
		}
	
		var arrayMarker = GetValue(aProps,'markers');
		if (arrayMarker == null)
		{
			arrayMarker = [];
		}	

		function onClickMarker()
		{
			var strLink = GetStringValue(GetValue(this,'link'));
			if (strLink != '')
			{
				//window.alert(strLink);
				window.location.href = strLink;
			}
		}
	
		var map = new google.maps.Map(
			document.getElementById(strSelector),
			{
				zoom: nZoom, 
				center: new google.maps.LatLng(fLat,fLong),  
				mapTypeId: strType,
				mapTypeControl: bTypeControl,
				streetViewControl: bStreetViewControl
			});

		arrayMarker.forEach( function(markerDef)
		{
			var strToolTip = GetStringValue(GetValue(markerDef,'tooltip'));
			var strLink = GetStringValue(GetValue(markerDef,'link'));
			
			var position = new google.maps.LatLng(GetValue(markerDef,'lat'),GetValue(markerDef,'long'));
			
			var fSize = GetValue(markerDef,'size');
			if (fSize < 0 || fSize > 1)
			{
				fSize = 1.0;
			}
	
			var fOpacity = GetNumberValue(GetValue(markerDef,'opacity'));
			if (fOpacity < 0 || fOpacity > 1)
			{
				fOpacity = 1.0;
			}
			
			var fScale = 10.0 + (40.0 * fSize);
			
			var strColor = GetStringValue(GetValue(markerDef,'color'));
			if (strColor == '')
			{
				strColor = 'red';
			}
			
			var marker = new google.maps.Marker(
			{
				position: position,
				map: map,
				title: strToolTip,
				icon:
				{
	        path: google.maps.SymbolPath.CIRCLE,
	        scale: fScale,
	        fillColor: strColor, 
	        fillOpacity: fOpacity,
	        strokeWeight: 0.4
    		} 
			});
			
			if (strLink != '')
			{
				marker.link = strLink;
				marker.addListener('click', onClickMarker);
			}
		});




	}
  
}

