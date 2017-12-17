module.exports = {
    get: get
};

function get (url, callback) {
    var xhr = new XMLHttpRequest();
    console.log('GET: ' + url);

    xhr.timeout = 20000;
    
    xhr.onreadystatechange = function (response) {
        if (xhr.readyState == 4 && xhr.status == 200) {
            callback(xhr.responseText);
        }
    };

    xhr.open('GET', url, true);
    xhr.ontimeout = function () {
        console.log('url timeout');
    };

    xhr.send();
}
