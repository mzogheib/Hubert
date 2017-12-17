var http = require('../utils/http');
var apiKey = require('../prv/google-maps-api-key');

module.exports = {
    reverseGeocode: reverseGeocode
};

var baseUrl = 'https://maps.googleapis.com/maps/api/';
var docType = 'json';

function reverseGeocode (lat, lng, callback) {
    var endpoint = 'geocode'
    var url = baseUrl + endpoint + '/' + docType + '?' + 'latlng=' + lat + ',' + lng + '&key=' + apiKey.key;

    http.get(url, callback);
}