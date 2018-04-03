// generate random crosshairs in the canvas.

(function(){


	document.addEventListener("keydown", (event) => {
		if(event.keyCode == 82) {
            isRecord = !isRecord;
        }
    });

    
	function generateRandomPoints(count, boundingBox){
		var points = [];
		for(i = 0; i < count; i++){
			var x = Math.random() * (boundingBox.x1 - boundingBox.x0) + boundingBox.x0;
			var y = Math.random() * (boundingBox.y1 - boundingBox.y0) + boundingBox.y0;
			points.push({x: Math.round(x), y: Math.round(y)});
		}
		return points;
	}

	function drawCrosshair(center, id){
		var g = d3.select("svg#canvas").append('g')
		                               .attr('id', id);
		var lineLength = 100;
		vx = center.x;
		vy = center.y - lineLength/2; // start points
		var vl = g.append("line")
		                     .attr("x1", vx)
		                     .attr("y1", vy)
		                     .attr("x2", vx)
		                     .attr("y2", vy + lineLength);
		hx = center.x - lineLength/2;
		hy = center.y;
		var hl = g.append("line")
		                     .attr('x1', hx)
		                     .attr('y1', hy)
		                     .attr('x2', hx + lineLength)
		                     .attr('y2', hy);
	}


	var width = $("svg#canvas").attr('width');
	var height = $("svg#canvas").attr('height');
	var padding = 100;

	var boundingBox = {
		x0 : 0 + padding,
		x1 : width - padding,
		y0 : 0 + padding,
		y1 : height - padding
	};

	var points = generateRandomPoints(10, boundingBox);

	//draw cross

	for(i = 0; i < points.length; i++){
		drawCrosshair(points[i], i);
	}
})();
