var WebSocket = require('ws');

var ws = new WebSocket("ws://localhost:10111/socket");

ws.on("open", function open() {
	// ws.send("hello world!");
	// ws.send("hello world again!");
	ws.send(new Buffer([1,2,3,4,5]), {binary: true});
	setTimeout(function() {
		ws.send(new Buffer([1,2,3,4,5]), {binary: true});	
	}, 100);
	setTimeout(function() {
		ws.send(new Buffer([1, 6, 10, 12, 15, 20, 100]), {binary: true});	
	}, 300);
	
	setInterval(function() {
		ws.ping();
	}, 5000);

	setTimeout(function() {
		console.log("closing!");
		ws.close();
	}, 60000);

});

ws.on("message", function(data, flags) {
	console.log(data);
});