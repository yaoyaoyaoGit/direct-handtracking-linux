(function(){
	document.addEventListener("keydown", (event) => {
	if(event.keyCode == 82) {
        isRecord = !isRecord;
    };
    })

    if("WebSocket" in window){
        console.log("WebSocket is supported by your Browser!");

        // Let us open a web socket
        var ws = new WebSocket("ws://localhost:9002/");

        ws.onopen = function()
        {
            // Web Socket is connected, send data using send()
        };

        ws.onmessage = function(evt){
            var receviedTouches = $.parseJSON(evt.data);
            if(receviedTouches == null){
                return;
            }
            else{
                console.log(receviedTouches);
            }
        };

        ws.onclose = function()
        {
            // websocket is closed.
            console.log("Connection is closed...");
        };
        window.onbeforeunload = function(event) {
            socket.close();
        };
    } 
    else {
            // The browser doesn't support WebSocket
        alert("WebSocket NOT supported by your Browser!");
    }

})();