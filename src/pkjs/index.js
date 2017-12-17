var Clay = require('pebble-clay');
var clayConfig = require('./clay/config');
var clay = new Clay(clayConfig, null, { autoHandleEvents: false });

var googleMapsApi = require('./google-maps/api');
var googleMapsUtils = require('./google-maps/utils'); 

var locationText = 'Unknown Location';

Pebble.addEventListener('showConfiguration', function(e) {
    // Reset message
    clay.setSettings({ MESSAGE: '' });
    Pebble.openURL(clay.generateUrl());
    window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
    
    function locationSuccess(pos) {
        googleMapsApi.reverseGeocode(pos.coords.latitude, pos.coords.longitude, function (response) {
            locationText = googleMapsUtils.parseRouteAndLocality(response);
        });
    }

    function locationError(err) {
        console.log('Error', err);
    }

    var locationOptions = {
        'timeout': 15000,
        'maximumAge': 60000
    }

});

Pebble.addEventListener('webviewclosed', function(e) {
    if (e && !e.response) {
        return;
    }

    // Get the keys and values from each config item
    var dict = clay.getSettings(e.response);
    // Add location manually as it is not handled by Clay 
    dict.LOCATION = locationText;

    // Send settings values to watch side
    Pebble.sendAppMessage(dict, function(e) {
        console.log('Sent config data to Pebble');
    }, function(e) {
        console.log('Failed to send config data!');
        console.log(JSON.stringify(e));
    });
});