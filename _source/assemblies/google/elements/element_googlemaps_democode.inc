$this->IncludeElement('googlemaps');
$googlemaps = new CGoogleMapsElement($this->GetWebsite(),'default');

$strOutput = $googlemaps->RenderGoogleMaps(
	51.477904,0,
	5,'satellite');

$strOutput .= $googlemaps->RenderGoogleMaps(
	50.046088,8.574422,
	10,'terrain');


$arrayMarkers = array(
	array(
			'lat' => 37.42228283989457,
			'long' => -122.08431500076773,   
			'tooltip' => 'Google',
			'size' => 0.5,
			'opacity' => 0.5,
			'color' => 'red',
			'link' => 'https://www.google.com'
		)
	);

$strOutput .= $googlemaps->RenderGoogleMaps(
	0,0,
	2,'satellite',
	false,false,
	$arrayMarkers);
