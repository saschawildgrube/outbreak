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
  
  RenderGoogleMaps(vSelector,aProps)
  {
  	Error('RenderGoogleMaps is NOT implemented!');	
  }
}

