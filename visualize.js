var margin = {top: 20, right: 20, bottom: 20, left: 40},
    width = 960 - margin.right,
    height = 540 - margin.top - margin.bottom;

var timeLineDims = {
    width: d3.select('#underlay').node().getBoundingClientRect().width,
    height: d3.select('#underlay').node().getBoundingClientRect().height + 40
};

// stuff for structure
var svg = d3.select('#chart-placeholder')
    .append('svg')        // create an <svg> element
    .attr('width', width) // set its dimensions
    .attr('height', height);

// allow ourselves to add markers to the svg
var markers_data = [
    { id: 0, name: 'circle', path: 'M 0, 0  m -5, 0  a 5,5 0 1,0 10,0  a 5,5 0 1,0 -10,0', viewbox: '-6 -6 12 12' }
  , { id: 1, name: 'square', path: 'M 0,0 m -5,-5 L 5,-5 L 5,5 L -5,5 Z', viewbox: '-5 -5 10 10' }
  , { id: 2, name: 'arrow', path: 'M 0,0 m -5,-5 L 5,0 L -5,5 Z', viewbox: '-5 -5 10 10' }
  , { id: 2, name: 'stub', path: 'M 0,0 m -1,-5 L 1,-5 L 1,5 L -1,5 Z', viewbox: '-1 -5 2 10' }
];
var defs = svg.append('svg:defs')
var markers = defs.selectAll('marker')
    .data(markers_data)
    .enter()
    .append('svg:marker')
      .attr('id', function(d){ return 'marker_' + d.name})
      .attr('markerHeight', 2)
      .attr('markerWidth', 2)
      .attr('markerUnits', 'strokeWidth')
      .attr('orient', 'auto')
      .attr('refX', 0)
      .attr('refY', 0)
      .attr('viewBox', function(d){ return d.viewbox }).append('svg:path')
        .attr('d', function(d){ return d.path })
        .attr('fill', "black");

var timeUnderlay = d3.select('#underlay')
      .append('svg')
      .attr('width', timeLineDims.width)
      .attr('height', timeLineDims.height);

var vid = d3.select("#soccervid")._groups[0][0];

d3.json('/data.json')
  .then(function(data) {
    var mData = JSON.parse(JSON.stringify(data));
    data.tracked = filterData(data.tracked);
    doDrawing(data);
    fixScale(data);
  });


function frameToTime(frame) {
  var framesPerSec = 25, secPerMin = 60, minPerHour = 60;
  var framesPerMin = framesPerSec*secPerMin,
      framesPerHour = framesPerMin*minPerHour;
  var hh = Math.floor(frame/framesPerHour);
  frame = frame - hh*framesPerHour;
  var mm = Math.floor(frame/framesPerMin);
  frame = frame - mm*framesPerMin;
  var ss = Math.floor(frame/framesPerSec);
  frame = frame - ss*framesPerSec;

  var twoDigitify = d3.format("0>2");

  return `${twoDigitify(hh)}:${twoDigitify(mm)}:${twoDigitify(ss)}.${twoDigitify(frame)}`
}

function durationToTime(duration) {
  var framesPerSec = 25;
  var secPerMin = 60, minPerHour = 60, secPerHour = secPerMin*minPerHour;
  var hh = Math.floor(duration/secPerHour);
  duration = duration - hh*secPerHour;
  var mm = Math.floor(duration/secPerMin);
  duration = duration - mm*secPerMin;
  var ss = Math.floor(duration);
  var frame = Math.ceil((duration-ss)*framesPerSec);

  var twoDigitify = d3.format("0>2");

  return `${twoDigitify(hh)}:${twoDigitify(mm)}:${twoDigitify(ss)}.${twoDigitify(frame)}`
}

function fixScale(data) {
  d3.select("#max-time").html(durationToTime(vid.duration));
  d3.select("#slider-time").attr("max", data.endFrame);
}

function findFramesOfIntersect(data, circle) {
  var timePoints = [];
  for (var i = 0; i < data.length; i++) {
    var player = data[i];
    for (var j = 0; j < player.path.length - 1; j++) {
      var segStart = player.path[j];
      var segEnd = player.path[j+1];
      var intersections = getIntersections(segStart, segEnd, circle);
      if (intersections.points || intersections.pointOnLine) {
        timePoints.push(player.path[j].time);
      }
    }
  }
  return timePoints;
}

function drawField(fieldType, dims) {
  // TODO want to get this info from the video. need to know field type, and have a dims with {min,max}{x,y}.

  // this draws a fakey soccer field for nowwww
  var fieldLayer = svg.append("g")
      .attr("class", "field");
  // layer for visibility
  fieldLayer.append("rect")
      .attr("x", 0)
      .attr("y", 0)
      .attr("width", width)
      .attr("height", height)
      .style("stroke-width", 0)
      .attr("fill", "#8BA870");
  // field outline
  fieldLayer.append("rect")
      .attr("x", 5)
      .attr("y", 5)
      .attr("width", width-10)
      .attr("height", height-10)
      .style("stroke", "#DDD")
      .style("stroke-width", 3)
      .attr("fill", "none");
  // goalie boxes
  fieldLayer.append("rect")
      .attr("x", 5)
      .attr("y", (height-300)/2+5)
      .attr("width", 200)
      .attr("height", 300)
      .style("stroke", "#DDD")
      .style("stroke-width", 3)
      .attr("fill", "none");
  fieldLayer.append("rect")
      .attr("x", width-5-200)
      .attr("y", (height-300)/2+5)
      .attr("width", 200)
      .attr("height", 300)
      .style("stroke", "#DDD")
      .style("stroke-width", 3)
      .attr("fill", "none");
  // half
  fieldLayer.append("rect")
      .attr("x", 5)
      .attr("y", 5)
      .attr("width", (width-10)/2)
      .attr("height", height-10)
      .style("stroke", "#DDD")
      .style("stroke-width", 3)
      .attr("fill", "none");
}

function offsetPathByVelocity(originalLine) {
  var offsetLine = JSON.parse(JSON.stringify(originalLine));
  for(var i = 0; i < offsetLine.length-1; i++) {
    // offset each point depending on the velocity at that point
    var velocity = distance(originalLine[i+1], originalLine[i]) /
                    (originalLine[i+1].time - originalLine[i].time);

    if (velocity == 0) {
      // dot isn't movin
      continue;
    }

    var scale = .5; // px per frame
    var travelVector = {x: originalLine[i+1].x - originalLine[i].x / velocity,
                        y: originalLine[i+1].y - originalLine[i].y / velocity};

    var offsetVector = {};

    if (travelVector.x != 0) {
      // find a perpendicular vector
      offsetVector = {x: -(travelVector.y)/travelVector.x,
                      y: 1};
      var offsetVectorLength = Math.sqrt(offsetVector.x*offsetVector.x + offsetVector.y*offsetVector.y);
      // normalize
      offsetVector.x /= offsetVectorLength;
      offsetVector.y /= offsetVectorLength;
      // now offset by speed of motion, scaled some
      offsetVector.x *= velocity/scale;
      offsetVector.y *= velocity/scale;
    } else {
      offsetVector = {x: 1, y: 0};
    }

    // now do the offset
    offsetLine[i].x += offsetVector.x;
    offsetLine[i].y += offsetVector.y;

    if (Number.isNaN(offsetLine[i].x) || Number.isNaN(offsetLine[i].y)) {
      // hm?
      offsetLine[i] = originalLine[i];
    }
  }
  // now do it again, but we gotta smooooooooooth it all out
  var smoothWid = 5;
  var smooth = function(idx) {
    var xSum = 0;
    var ySum = 0;
    for (var i = idx-smoothWid; i <= idx+smoothWid; i++) {
      xSum += offsetLine[i].x;
      ySum += offsetLine[i].y;
    }
    return { x: xSum / (smoothWid*2+1), y: ySum / (smoothWid*2+1) };
  }
  for (var i = smoothWid; i < offsetLine.length-smoothWid; i++) {
    // to smooth... we average the smoothWid points in front and behind.
    offsetLine[i] = smooth(i);
  }

  return offsetLine;
}

function clearDrawing(drawing) {
  drawing.selectAll("*").remove();
}

function doDrawing(data) {
  // the base of this visualization a field illustration
  drawField(data.sport, data.fieldDims);

  // define the scales that we use to map our x y things into screen space
  var x = d3.scaleLinear()
    .range([0, width])
    .domain([0, data['width']]);
  var unx = d3.scaleLinear()
    .domain([0, width])
    .range([0, data['width']]);
  var y = d3.scaleLinear()
    .range([height, 0])
    .domain([data['height'], 0]);
  var uny = d3.scaleLinear()
    .domain([height, 0])
    .range([data['height'], 0]);
  function color(d) { return "rgb(" + d.color.r + "," + d.color.g + "," + d.color.b + ")"; };
  function opacity(d, frame) {
    if (frame < d.born || frame > d.died)
      return 0.0;
    return 1.0;
  }
  function key(d) { return d.label; }

  var line = d3.line()
    .x(function(d) { return x(d.x); })
    .y(function(d) { return y(d.y); })
    .curve(d3.curveCardinal); // make it a li'l curvy

  // for data interpolation (when we skip frames)
  var bisect = d3.bisector(function(d) { return d.time; });

  // listen to the slider
  d3.select("#slider-time")
    .on("mousemove", function() {
      displayTime(parseInt(this.value));
    });

  // create all the groups, and populate some with data!

  var interactionUnderlay = svg.append('g')
      .attr("class", "interaction");

  var paths = svg.append("g")
      .attr("class", "paths");


  // now we want the active paths to show up as more loud. what is an active path, you ask? well, it's a path that represents the 5s around the position of the scrubber thingy in the video seeker widget.

  var activePaths = svg.append("g")
      .attr("class", "activepaths")
      .selectAll("path")
      .data(interpolateData(0))
      .enter()
      .append('path')
        .attr('d', function(d) {
            return getVelocityOffsetSVGPath(d.path);
          })
        .attr('fill', function(d) { return color(d); })
        .attr("stroke", function(d) { return color(d); })
        .attr('opacity', 1.0);

  var dot = svg.append("g")
      .attr("class", "playermarkers")
      .selectAll(".playermarker")
      .data(interpolateData(0))
      .enter().append("path")
        .attr("class", "playermarker")
        .attr('marker-end', 'url(#marker_arrow)')
        .attr("stroke", "black")
        .attr("stroke-width", 5)
        .attr("d", function(d) {return line([{"x":d.prevX, "y":d.prevY},{"x":d.x, "y":d.y}]); })
        .style("opacity", function(d) { return d.opacity; })
        .call(positionFade);

  // draw a label at the current position of each labelled thingy
  var text = svg.append("g")
      .attr("class", "label")
      .selectAll(".label")
      .data(interpolateData(0))
      .enter().append("text")
        .attr("class", "label")
        .style("stroke", "black")
        .attr("text-anchor", "middle")
        .text(function(d) { return key(d); })
        .call(positionText);

  // stuff for the time brush! that lil thing at the bottom that lets you select a larger period of time for highlightin'
  var timeBrush = d3.brushX()
    .extent([[0, 0], [timeLineDims.width, timeLineDims.height]])
    .on("start brush", brushed);

  timeUnderlay.append("g")
    .call(timeBrush)
    .call(timeBrush.move, [0, 100].map(x))
    .selectAll(".overlay")
      .each(function(d) { d.type = "selection"; }) // Treat overlay interaction as move.
      .on("mousedown touchstart", brushcentered); // Recenter before brushing.

  function clipPathToRange(low, high) {
    return data.tracked.map(function(d) {
      var filteredPath = d.path.filter(function (elt) {
        return elt.time >= low && high >= elt.time;
      });

      return {
        label: d.label,
        color: color(d),
        path: filteredPath
      };
    });
  }

  // Interpolates the dataset for the given frame.
  // we clip the "path" to a neighborhood of size 5s*30fps
  function interpolateData(frame) {
    var range = 3*30*2;
    return data.tracked.map(function(d) {
      var eventHorizonLow = frame - range/2;
      var eventHorizonHigh = frame + range/2;
      var filteredPath = d.path.filter(function (elt) {
        return elt.time >= eventHorizonLow && eventHorizonHigh >= elt.time;
      });

      return {
        label: d.label,
        x: interpolateValues(d.path, frame, "x"),
        y: interpolateValues(d.path, frame, "y"),
        color: color(d),
        opacity: opacity(d, frame),
        path: filteredPath,
        prevX: interpolateValues(d.path, frame-1, "x"),
        prevY: interpolateValues(d.path, frame-1, "y")
      };
    });
  }

  // Finds (and possibly interpolates) the value for the specified frame.
  function interpolateValues(values, frame, coord) {
    var i = bisect.left(values, frame, 0, values.length - 1),
        a = values[i];
    if (i > 0) {
      var b = values[i - 1],
          t = (frame - a.time) / (b.time - a.time);
      return a[coord] * (1 - t) + b[coord] * t;
    }
    return a[coord];
  }

  // Updates the display to show the specified frame / time.
  function displayTime(frame) {
    var interpolated = interpolateData(frame);
    dot.data(interpolated)
        .call(positionFade);
    text.data(interpolated)
        .call(positionText);
    activePaths.data(interpolated)
        .call(activateNearbyPaths);
    vid.play()
      .then(function() {
        vid.currentTime = frame/d3.select("#slider-time").attr("max") * vid.duration;
        vid.pause();
      });
  }

  function positionFade(obj) {
    obj.attr("d", function(d) { return line([{"x":d.prevX, "y":d.prevY},{"x":d.x, "y":d.y}]); })
       .style("opacity", function(d) { return d.opacity; })
       .attr("stroke-width", 5)
       .attr("marker-end", "url(#marker_arrow)")
       .attr("stroke", "black");
  }

  function positionText(obj) {
    obj.attr("x", function(d) { return x(d.x); })
       .attr("y", function(d) { return y(d.y); })
       .style("opacity", function(d) { return d.opacity; });
  }

  function activateNearbyPaths(obj) {
    obj.attr("d", function(d) { return getVelocityOffsetSVGPath(d.path); })
      .attr("fill", function(d) { return d.color; });
  }

  // draw a line for each labelled thingy
  var linez = paths.selectAll("path").data(data.tracked);

  function getVelocityOffsetSVGPath(path) {
    var offsetLine = offsetPathByVelocity(path);
    // put them together; make sure to reverse the offset so we don't skip back to front
    return line(path.concat(offsetLine.reverse()));
  }

  var newLinez = linez.enter();
  newLinez.append('path')
    .attr('d', function(d) {
      return getVelocityOffsetSVGPath(d.path);
    })
    .attr('fill', function(d) { return color(d); }) // we have to fill it now. :(
    .attr("stroke", function(d) { return color(d); })
    .attr('opacity', .15)
    ;

  // title it
  svg.append("text")
      .attr("x", (width / 2))
      .attr("y", 30)
      .attr("text-anchor", "middle")
      .style("font-size", "46px")
      .text(data['sport']);


  svg.on("click", function () {
      var mouse = d3.mouse(this);
      var x = mouse[0];
      var y = mouse[1];
      var r = 20;

      clearDrawing(interactionUnderlay);

      interactionUnderlay
          .append("circle")
          .attr("cx", mouse[0])
          .attr("cy", mouse[1])
          .style("fill", "#FFF")
          .style("opacity", ".3")
          .attr("r", r);

      clearDrawing(timeUnderlay);

      var timesToHighlight = findFramesOfIntersect(data.tracked, {x: unx(x), y: uny(y), r: unx(r)});

      for (var i = 0; i < timesToHighlight.length; i++) {
        var time = timesToHighlight[i];
        timeUnderlay.append("rect")
          .attr("x", time * timeLineDims.width / data.endFrame)
          .attr("y", 0)
          .attr("width", timeLineDims.width / data.endFrame)
          .attr("height", timeLineDims.height)
          .style("fill", "#F00");
      }

  });

  //timebrush stuff!!
  function brushcentered() {
    var dx = x(100) - x(0), // Use a fixed width when recentering.
        cx = d3.mouse(this)[0],
        x0 = cx - dx / 2,
        x1 = cx + dx / 2;
    d3.select(this.parentNode).call(brush.move, x1 > width ? [width - dx, width] : x0 < 0 ? [0, dx] : [x0, x1]);
  }

  function brushed() {
    var extent = d3.event.selection.map(x.invert, x);
    var selectedPaths = clipPathToRange(extent[0], extent[1]);
    activePaths.data(selectedPaths)
        .call(activateNearbyPaths);
  }

  displayTime(parseInt(d3.select("#slider-time").attr("value")));
}
