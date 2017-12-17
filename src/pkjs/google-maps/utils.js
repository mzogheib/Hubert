var _ = require('lodash');

module.exports = {
    parseRouteAndLocality: parseRouteAndLocality
};

function parseRouteAndLocality (responseJSONString) {
    var response = JSON.parse(responseJSONString);
    if (response && response.results.length === 0) {
        return '';
    }

    var route = _.find(response.results[0].address_components, function (component) {
        return _.includes(component.types, 'route');
    });

    var locality = _.find(response.results[0].address_components, function (component) {
        return _.includes(component.types, 'locality');
    });

    return route.short_name + ', ' + locality.short_name;
}