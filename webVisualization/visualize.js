var margin = {top: 20, right: 20, bottom: 20, left: 40},
    width = 960 - margin.right,
    height = 540 - margin.top - margin.bottom;

var timeLineDims = {
    width: d3.select('#underlay').node().getBoundingClientRect().width,
    height: d3.select('#underlay').node().getBoundingClientRect().height
};

// stuff for structure
var svg = d3.select('#chart-placeholder')
    .append('svg')        // create an <svg> element
    .attr('width', width) // set its dimensions
    .attr('height', height);

var timeUnderlay = d3.select('#underlay')
      .append('svg')
      .attr('width', timeLineDims.width)
      .attr('height', timeLineDims.height);

var vid = d3.select("#soccervid")._groups[0][0];

//d3.json('/data.json')
d3.json('/data.json')
  .then(function(data) {
    var mData = JSON.parse(JSON.stringify(data));
    data.tracked = filterData(data.tracked);
    fixScale(data);
    clearDrawing(svg);
    clearDrawing(timeUnderlay);
    doDrawing(data);
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
  svg.append("rect")
      .attr("x", 5)
      .attr("y", 5)
      .attr("width", width-10)
      .attr("height", height-10)
      .style("stroke", "#aaa")
      .style("stroke-width", 3)
      .attr("fill", "none");
  // goalie boxes
  svg.append("rect")
      .attr("x", 5)
      .attr("y", (height-300)/2+5)
      .attr("width", 200)
      .attr("height", 300)
      .style("stroke", "#aaa")
      .style("stroke-width", 3)
      .attr("fill", "none");
  svg.append("rect")
      .attr("x", width-5-200)
      .attr("y", (height-300)/2+5)
      .attr("width", 200)
      .attr("height", 300)
      .style("stroke", "#aaa")
      .style("stroke-width", 3)
      .attr("fill", "none");
  // half
  svg.append("rect")
      .attr("x", 5)
      .attr("y", 5)
      .attr("width", (width-10)/2)
      .attr("height", height-10)
      .style("stroke", "#aaa")
      .style("stroke-width", 3)
      .attr("fill", "none");
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
    return (frame < d.born || frame > d.died)? 0.0 : 1.0;
  }
  function key(d) { return d.label; }

  // for data interpolation (when we skip frames)
  var bisect = d3.bisector(function(d) { return d.time; });

  // listen to the slider
  d3.select("#slider-time")
    .on("mousemove", function() {
      displayTime(parseInt(this.value));
    });

  var dot = svg.append("g")
      .attr("class", "dots")
      .selectAll(".dot")
      .data(interpolateData(0))
      .enter().append("circle")
        .attr("class", "dot")
        .style("fill", function(d) { return color(d); })
        .style("opacity", function(d) { return opacity(d, 1); })
        .attr("r", 6)
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

  // Interpolates the dataset for the given frame.
  function interpolateData(frame) {
    return data.tracked.map(function(d) {
      return {
        label: d.label,
        x: interpolateValues(d.path, frame, "x"),
        y: interpolateValues(d.path, frame, "y"),
        color: d.color,
        opacity: opacity(d, frame)
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
    dot.data(interpolateData(frame), key)
        .call(positionFade);
    text.data(interpolateData(frame), key)
        .call(positionText);
    var vid = d3.select("#soccervid")._groups[0][0];
    vid.play()
      .then(function() {
        vid.currentTime = frame/d3.select("#slider-time").attr("max")*vid.duration;
        vid.pause();
      });
  }

  function positionFade(obj) {
    obj.attr("cx", function(d) { return x(d.x); })
       .attr("cy", function(d) { return y(d.y); })
       .style("opacity", function(d) { return d.opacity; });
  }

  function positionText(obj) {
    obj.attr("x", function(d) { return x(d.x); })
       .attr("y", function(d) { return y(d.y); })
       .style("opacity", function(d) { return d.opacity; });
  }

  // draw a line for each labelled thingy
  var linez = svg.selectAll("path").data(data.tracked);
  var line = d3.line()
    .x(function(d) { return x(d.x); })
    .y(function(d) { return y(d.y); })
    //.curve(d3.curveCardinal); // make it a li'l curvy (don't do this; it kinda fucks with intersection calculations... :( ))
  var newLinez = linez.enter();
  newLinez.append('path')
    .attr('d', function(labelled) {
      return line(labelled.path);
    })
    .attr('fill', "none") // don't fill it, that's fucking annoying
    .attr("stroke", function(d) { return color(d); })
    .attr('opacity', .3)
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
      svg
          .append("circle")
          .attr("cx", mouse[0])
          .attr("cy", mouse[1])
          .style("fill", "#777")
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
}
