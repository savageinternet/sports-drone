d3.json('/data.json')
  .then(function(data) {
    data = filterData(data);

    var wid = 1280;
    var ht = 720;

    // stuff
    var svg = d3.select('body')
      .append('svg')        // create an <svg> element
        .attr('width', wid) // set its dimentions
        .attr('height', ht);

    var linez = svg.selectAll("path").data(data);
    var newLinez = linez.enter();

    // TODO : we should actually capture the maxX and maxY data from the video footage. our likely ranges are basically the pixel sizes of the videos we are working with, so 720p 1080p and 4k
    var maxX = d3.max(data, function(labelled, idx) {
      return d3.max(labelled.path, function(point, idx) {
        return point[0];
      });
    });
    console.log(maxX);

    var maxY = d3.max(data, function(labelled, idx) {
      return d3.max(labelled.path, function(point, idx) {
        return point[1];
      });
    });
    console.log(maxY);

    var x = d3.scaleLinear()
      .range([0, wid])
      .domain([0, maxX]);
    var y = d3.scaleLinear()
      .range([0, ht])
      .domain([maxY, 0]);

    var line = d3.line()
      .x(function(d) { return x(d[0]); })
      .y(function(d) { return y(d[1]); });

    newLinez.append('path')
      .attr('d', function(labelled) {
        return line(labelled.path);
      })
      .attr('fill', "none")
      .attr("stroke", function() {
          return "hsl(" + Math.random() * 360 + ",100%,50%)";
      });

  });

function nearby(ptA, ptB) {
  var sensibleDistance = 20.0; // the unit here is pixels.
  return Math.sqrt(Math.pow(ptA[0]-ptB[0],2)+Math.pow(ptA[1]-ptB[1],2)) < sensibleDistance;
}

function filterData(data) {
  // first, let's try to join them up into bigger things. we'll see if we have several parts that make up a single object's path. this can happen if someone stands still for a while and the CV algorithm forgets about them before they move again.

  // TODO: this doesn't take into account the phenomenon of "2-1-2", where players' blobs can get joined together and then separate

  // an array in which we'll track things we are throwing away
  var removed = [];
  for(var i=0; i < data.length; i++) {
    if (i in removed) continue; // don't look at stuff we've "removed"
    var pathIEnd = data[i].path[data[i].path.length - 1];
    // we'll just check the ones forward of where we are. this way we know we can just look at if our path ends where the next one starts (since j will come after I temporally), rather than looking at it both ways.
    if (data[i].died) { // only try to extend if the point actually died.
      for (var j=i+1; j < data.length; j++) {
        if (data[j].born < data[i].died) continue; // violates temporality
        var pathJStart = data[j].path[0];
        if (nearby(pathIEnd,pathJStart)) {
          data[i].path = data[i].path.concat(data[j].path);
          data[i].died = data[j].died; // be sure to update this!
          removed.push(j);
          // note that we don't break the loop here. we can keep extending a single path.
        }
      }
    }
  }
  // now actually remove all the stuff in removed; we start at the back so as not to screw up our indices
  for(var rem_i in removed.reverse()) {
    data.splice(rem_i,1);
  }
  removed = [];

  // now pull out all the really short ones... we'll say that "short" is <64 points/frames, which would be 2 seconds of typical footage
  var shortPathLength = 64;
  // we could have done this alongside the other thing (at the end of each extending loop), but... that's just confusing to read.
  for(var i=0; i < data.length; i++) {
    if (data[i].path.length <= shortPathLength) {
      removed.push(i);
    }
  }
  // now actually remove all the stuff in removed; we start at the back so as not to screw up our indices
  for(var rem_i in removed.reverse()) {
    data.splice(rem_i,1);
  }
  removed = [];

  return data;
}
