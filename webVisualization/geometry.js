// from https://bl.ocks.org/milkbread/11000965

function getIntersections(a, b, c) {
  // if either a or b is with c.r of (c.x,c.y), then... yes!
  if (distance(a,c) < c.r || distance(b,c) < c.r) return {points: true, pointOnLine: false};

	// Calculate the euclidean distance between a & b
	var eDistAtoB = distance(a,b);

	// compute the direction vector d from a to b
	var d = { x: (b.x-a.x)/eDistAtoB, y: (b.y-a.y)/eDistAtoB };

	// Now the line equation is x = dx*t + ax, y = dy*t + ay with 0 <= t <= 1.

	// compute the value t of the closest point to the circle center (cx, cy)
	var t = (d.x * (c.x-a.x)) + (d.y * (c.y-a.y));

  if (t > 1 || t < 0) { // then it could be on the extension of the line, but it's not on the line segment, so just return
    return {points: false, pointOnLine: false};
  }

	// compute the coordinates of the point e on line and closest to c
  var e = {coords:[], onLine:false};
	e.coords.x = (t * d.x) + a.x;
	e.coords.y = (t * d.y) + a.y;

	// Calculate the euclidean distance between c & e
	var eDistCtoE = Math.sqrt( Math.pow(e.coords.x-c.x, 2) + Math.pow(e.coords.y-c.y, 2) );

	// test if the line intersects the circle
	if( eDistCtoE < c.r ) {
		// compute distance from t to circle intersection point
	    dt = Math.sqrt( Math.pow(c[2], 2) - Math.pow(eDistCtoE, 2));

	    // compute first intersection point
	    var f = {coords:[], onLine:false};
	    f.coords.x = ((t-dt) * d.x) + a.x;
	    f.coords.y = ((t-dt) * d.y) + a.y;
	    // check if f lies on the line
	    f.onLine = is_on(a,b,f.coords);

	    // compute second intersection point
	    var g = {coords:[], onLine:false};
	    g.coords.x = ((t+dt) * d.x) + a.x;
	    g.coords.y = ((t+dt) * d.y) + a.y;
	    // check if g lies on the line
	    g.onLine = is_on(a,b,g.coords);

		return {points: {intersection1:f, intersection2:g}, pointOnLine: false};

	} else if (parseInt(eDistCtoE) === parseInt(c.r)) {
		// console.log("Only one intersection");
		return {points: false, pointOnLine: e};
	} else {
		// console.log("No intersection");
		return {points: false, pointOnLine: false};
	}
}

function distance(ptA, ptB) {
  return Math.sqrt(Math.pow(ptA.x-ptB.x,2)+Math.pow(ptA.y-ptB.y,2));
}

function nearby(ptA, ptB) {
  var sensibleDistance = 10.0; // the unit here is pixels.
  return distance(ptA,ptB) < sensibleDistance;
}

function lineLength(path) {
  var sumDistance = 0;
  for (var i = 0; i < path.length - 1; i++) {
    sumDistance += distance(path[i], path[i+1]);
  }
  return sumDistance;
}

function is_on(a, b, c) {
	return distance(a,c) + distance(c,b) == distance(a,b);
}

function getAngles(a, b, c) {
	// calculate the angle between ab and ac
	angleAB = Math.atan2( b.y - a.y, b.x - a.x );
	angleAC = Math.atan2( c.y - a.y, c.x - a.x );
	angleBC = Math.atan2( b.y - c.y, b.x - c.x );
	angleA = Math.abs((angleAB - angleAC) * (180/Math.PI));
	angleB = Math.abs((angleAB - angleBC) * (180/Math.PI));
	return [angleA, angleB];
}
