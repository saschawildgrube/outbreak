'use strict';

	class OutbreakMapReactComponent extends GoogleMapsReactComponent
	{
		RenderGoogleMaps(strSelector,aProps)
		{
			var arrayOutbreakData = GetValue(aProps,'data');
			if (arrayOutbreakData == null)
			{
				arrayOutbreakData = [];
			}
			
			var arrayCountries = GetValue(arrayOutbreakData,'countries');
			if (arrayCountries == null)
			{
				arrayCountries = [];
			}
			
			var arrayShips = GetValue(arrayOutbreakData,'ships');
			if (arrayShips == null)
			{
				arrayShips = [];
			}
			
			var arrayMap = GetValue(arrayOutbreakData,'map');
			if (arrayMap == null)
			{
				arrayMap = [];	
			}
			
			var fLat = GetValue(arrayMap,'lat');
			if (fLat == null)
			{
				fLat = 30;
			}
			var fLong = GetValue(arrayMap,'long');
			if (fLong == null)
			{
				fLong = 0;
			}
			
			var nZoom = GetValue(arrayMap,'zoom');
			if (nZoom == null)
			{
				nZoom = 2;
			}
			
			var strType = GetValue(arrayMap,'type');
			if (strType == null)
			{
				strType = 'terrain';
			}
	
			var map = new google.maps.Map(
				document.getElementById(strSelector),
				{
	  			zoom: nZoom, 
	  			center: new google.maps.LatLng(fLat,fLong),  
	  			//mapTypeId: 'satellite',
	  			mapTypeId: strType,
	  			mapTypeControl: false,
	  			streetViewControl: false
				});
	
			var nConfirmedMax = 0;
			var nConfirmedMin = 0;
			arrayCountries.forEach( function(arrayCountry)
			{
				var nConfirmed = GetValue(arrayCountry,'confirmed');
				nConfirmedMax = Math.max(nConfirmed,nConfirmedMax);
				nConfirmedMin = Math.min(nConfirmed,nConfirmedMax);
	
			});
			
			arrayCountries.forEach( function(arrayCountry)
			{
				var fMarkerSize = GetValue(arrayCountry,'confirmed') / nConfirmedMax;
				arrayCountry['markersize'] = fMarkerSize;
				
			},this);
	
			arrayCountries.forEach( function(arrayCountry)
			{
				var nConfirmed = GetValue(arrayCountry,'confirmed');
				var nDeaths = GetValue(arrayCountry,'deaths');
				var nRecovered = GetValue(arrayCountry,'recovered');
				var fMortality = GetValue(arrayCountry,'mortality');
				var strMortality = GetValue(arrayCountry,'text_mortality');
				
				var strToolTip =
					GetValue(arrayCountry,'name')+'\n'
					+'Confirmed: '+GetValue(arrayCountry,'text_confirmed')+'\n'
					+'Deaths: '+GetValue(arrayCountry,'text_deaths')+'\n'
					+'Mortality: '+GetValue(arrayCountry,'text_mortality')+'\n'
					+'Recovered: '+GetValue(arrayCountry,'text_recovered')+'\n'
					+'Active: '+GetValue(arrayCountry,'text_active');
				
				var position = new google.maps.LatLng(arrayCountry['lat'],arrayCountry['long']);
				
				var fMarkerSize = GetValue(arrayCountry,'markersize');
				var fRadius = fMarkerSize * 500 * 1000;
				
				var strColor = 'green';
				if (nConfirmed > 0)
				{
					if (nDeaths > 0)
					{
						strColor = 'red';
					}
					else
					{
						strColor = 'orange';
					}
				}
				
				var marker = new google.maps.Marker(
				{
					position: position,
					map: map,
					title: strToolTip,
					icon:
					{
		        path: google.maps.SymbolPath.CIRCLE,
		        scale: 10.0 + (40.0 * fMarkerSize),    
		        fillColor: strColor, 
		        fillOpacity: 0.5,
		        strokeWeight: 0.4
	    		},
				});
				
			
			});



			arrayShips.forEach( function(arrayShip)
			{
				var nConfirmed = GetValue(arrayShip,'confirmed');
				var nDeaths = GetValue(arrayShip,'deaths');
				var nRecovered = GetValue(arrayShip,'recovered');
				
				var strToolTip =
					'Ship: '+GetValue(arrayShip,'name')+'\n'
					+'Confirmed: '+GetValue(arrayShip,'text_confirmed')+'\n'
					+'Deaths: '+GetValue(arrayShip,'text_deaths')+'\n'
					+'Mortality: '+GetValue(arrayShip,'text_mortality')+'\n'
					+'Recovered: '+GetValue(arrayShip,'text_recovered');
				
				var position = new google.maps.LatLng(arrayShip['lat'],arrayShip['long']);
				
				var strColor = 'green';
				if (nConfirmed > 0)
				{
					if (nDeaths > 0)
					{
						strColor = 'red';
					}
					else
					{
						strColor = 'yellow';
					}
				}
				
				var marker = new google.maps.Marker(
				{
					position: position,
					map: map,
					title: strToolTip,
					icon:
					{
		        path: google.maps.SymbolPath.FORWARD_CLOSED_ARROW,
		        scale: 7.0,      
		        fillColor: strColor, 
		        fillOpacity: 0.5,
		        strokeWeight: 0.4
	    		},
				});
				
				
			});

		/*		
				
				var circle = new google.maps.Circle(
					{
		        strokeColor: '#FF0000',
		        strokeOpacity: 0.8,
		        strokeWeight: 2,
		        fillColor: '#FF0000',
		        fillOpacity: 0.35,
		        map: map,
		        center: position,
		        radius: fRadius
		      });
			*/	


	
			// Source:
			// https://bl.ocks.org/mbostock/899711
	
	/*
	
	html, body, #map {
	  width: 100%;
	  height: 100%;
	  margin: 0;
	  padding: 0;
	}
	
	.stations, .stations svg {
	  position: absolute;
	}
	
	.stations svg {
	  width: 60px;
	  height: 20px;
	  padding-right: 100px;
	  font: 10px sans-serif;
	}
	
	.stations circle {
	  fill: brown;
	  stroke: black;
	  stroke-width: 1.5px;
	}
	
	*/
	
	/*
	
			var overlay = new google.maps.OverlayView();
			overlay.onAdd = function()
			{
				Trace('Outbreakmap: Overlay.onAdd');
	
		  	var sLayer = d3.select(this.getPanes().overlayLayer)
		  		.append("div")
		      	.attr('class', 'nations');
	
	
	  		overlay.draw = function()
	  		{
	    		var projection = this.getProjection();
	    		var nPadding = 10;
	
					var sMarker = sLayer.selectAll("svg")
						.data(arrayAreas)
	          	.each(transform) // update existing markers
	        	.enter()
	        		.append("svg")
	          		.each(transform)
	          		.style('position', 'absolute');
	
	      	// Add a circle.
	      	sMarker.append('circle')
	          .attr('r', 3)
	          .attr('cx', nPadding)
	          .attr('cy', nPadding);
	
		      // Add a label.
		      sMarker.append('text')
		          .attr('x', nPadding + 7)
		          .attr('y', nPadding)
		          .attr('dy', '.31em')
		          .text( function(arrayArea)
		          	{
		          		
		          		var strText =
										GetValue(arrayArea,'name')+' '
										+'Confirmed: '+arrayArea['confirmed']+' '
										+'Deaths: '+arrayArea['deaths']+' '
										+'Mortality: '+arrayArea['mortality'];
		          		Trace(strText);
		          		return strText;
		          	});
	
		      function transform(arrayArea)
		      {
		      	//Trace('transform: arrayArea: '+RenderValue(arrayArea));
		        var position = new google.maps.LatLng(arrayArea['lat'], arrayArea['long']);
		        //Trace('transform: position: '+position);
		        var pixel = projection.fromLatLngToDivPixel(position);
		        //Trace('transform: pixel: '+pixel);
		        return d3.select(this)
		            .style('left', (pixel.x - nPadding) + 'px')
		            .style('top', (pixel.y - nPadding) + 'px');
		      }
		    };
		    
	  	};
	  	overlay.setMap(map);
	
	*/
	
		
		}	
	}
	
