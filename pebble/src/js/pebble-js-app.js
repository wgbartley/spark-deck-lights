var DEVICE_ID = 'YOUR_DEVICE_ID';
var ACCESS_TOKEN = 'YOUR_ACCESS_TOKEN';
var API_URL = 'https://api.spark.io/v1/devices/'+DEVICE_ID;

var relays = {
	1: false,
	2: false,
	3: false,
	4: false
};

Pebble.addEventListener('ready', function(e) {
	console.log('PebbleJS ready!');
});


Pebble.addEventListener('appmessage', function(dict) {
	if(dict.payload!=undefined) {
		for(var i in dict.payload) {
			console.log('Received key: '+i);

			switch(i.toUpperCase()) {
				case 'KEY_RELAYS':
					getRelayStates();
					break;

				case 'KEY_OUTLET1':
					toggleRelay(1);
					break;

				case 'KEY_OUTLET2':
					toggleRelay(2);
					break;

				case 'KEY_OUTLET3':
					toggleRelay(3);
					break;

				case 'KEY_OUTLET4':
					toggleRelay(4);
					break;

				case 'KEY_TEMPERATURE':
				default:
					getTempF();
					break;
			}

			break;
		}
	}
});


function appReply(data) {
	Pebble.sendAppMessage(
	data,
	function(e) {
		console.log('Sent: '+JSON.stringify(data));
	}, function(e) {
		console.log('Fail: '+JSON.stringify(data));
	});
}


function toggleRelay(i) {
	// Assume we want to turn it on
	var relayState = 'on';

	// If it's already on, we will want to turn it off
	if(isRelayOn(i)===true)
		relayState = 'off';

	post('/toggleRelay', 'args=relay'+i+relayState, function(resp) {
		resp = JSON.parse(resp);

		parseStates(resp.return_value);

		appReply({'KEY_RELAYS':resp.return_value});
	});
}


function isRelayOn(i) {
	return relays[i];
}


function getRelayStates() {
	get('/relayStates', function(resp) {
		resp = JSON.parse(resp);

		parseStates(resp.result);

		appReply({'KEY_RELAYS':resp.result});
	});
}


function parseStates(i) {
	// Convert the integer into a binary string representation
	var states = (i).toString(2);

	// Pad leading 0s
	while(states.length<4)
		states = "0"+states;

	// Loop through the relay states
	for(var j=0; j<4; j++) {
		// If the relay is on
		if(states[j]==1)
			relays[(j+1)] = true;
		else
			relays[(j+1)] = false;
	}
}


function getTempF() {
	get('/tempF', function(resp) {
		resp = JSON.parse(resp);

		appReply({'KEY_TEMPERATURE':Math.round(resp.result)});
	});
}


function get(url, cb) {
	url = API_URL+url+'?access_token='+ACCESS_TOKEN;

	var xmlhttp = new XMLHttpRequest();
	
	xmlhttp.onreadystatechange = function() {
		if(xmlhttp.readyState==4 && xmlhttp.status==200) {
			cb(xmlhttp.responseText);
		}
	}

	xmlhttp.open('GET', url, true);
	xmlhttp.send();
}


function post(url, params, cb) {
	url = API_URL+url+'?access_token='+ACCESS_TOKEN;

	var xmlhttp = new XMLHttpRequest();

	xmlhttp.open('POST', url, true);
	xmlhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
	xmlhttp.setRequestHeader('Content-length', params.length);
	xmlhttp.setRequestHeader('Connection', 'close');

	xmlhttp.onreadystatechange = function() {
		if(xmlhttp.readyState==4 && xmlhttp.status==200) {
			cb(xmlhttp.responseText);
		}
	}

	xmlhttp.send(params);
}
