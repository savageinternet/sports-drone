var margin = {top: 20, right: 20, bottom: 20, left: 40},
    width = 960 - margin.right,
    height = 540 - margin.top - margin.bottom;

var mData;

// stuff
var svg = d3.select('body')
    .append('svg')        // create an <svg> element
    .attr('width', width) // set its dimentions
    .attr('height', height);

d3.json('/data.json')
  .then(function(data) {
    mData = JSON.parse(JSON.stringify(data));
    data.tracked = filterData(data.tracked);
    fixScale(data);
    clearDrawing();
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
  return `${d3.format("0>2")(hh)}:${d3.format("0>2")(mm)}:${d3.format("0>2")(ss)}.${d3.format("0>2")(frame)}`;
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
  return `${d3.format("0>2")(hh)}:${d3.format("0>2")(mm)}:${d3.format("0>2")(ss)}.${d3.format("0>2")(frame)}`
}

function fixScale(data) {
  var vid = d3.select("#soccervid")._groups[0][0];
  d3.select("#max-time").html(durationToTime(vid.duration));

  d3.select("#slider-time").attr("max", data.endFrame);
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

function clearDrawing() {
  svg.selectAll("*").remove();
}

function doDrawing(data) {
  // the base of this visualization a field illustration
  drawField(data.sport, data.fieldDims);

  // define the scales that we use to map our x y things into screen space
  var x = d3.scaleLinear()
    .range([0, width])
    .domain([0, data['width']]);
  var y = d3.scaleLinear()
    .range([height, 0])
    .domain([data['height'], 0]);
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
      .data(interpolateData(1))
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
      .data(interpolateData(1))
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

  // Updates the display to show the specified year.
  function displayTime(frame) {
    dot.data(interpolateData(frame), key)
        .call(positionFade);
    text.data(interpolateData(frame), key)
        .call(positionText);
    var vid = d3.select("#soccervid")._groups[0][0];
    vid.play()
      .then(function() {
        vid.currentTime = (frame-1)/d3.select("#slider-time").attr("max")*vid.duration;
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
    .curve(d3.curveCardinal); // make it a li'l curvy
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

function removeIdxsFromArray(idxs, arr) {
  idxs.sort(function(a,b) { return a - b });
  for(var rem_i of idxs.reverse()) {
    arr.splice(rem_i,1);
  }
  return arr;
}

function stringTogether(data) {
  var remove = [];

  // join them up into bigger things. we'll see if we have several parts that make up a single object's path. this can happen if someone stands still for a while and the CV algorithm forgets about them before they move again.
  for(var i=0; i < data.length; i++) {
    if (remove.includes(i)) continue; // don't look at stuff we've "removed"
    var pathIEnd = data[i].path[data[i].path.length - 1];
    // we'll just check the ones forward of where we are. this way we know we can just look at if our path ends where the next one starts (since j will come after I temporally), rather than looking at it both ways.
    for (var j=i+1; j < data.length; j++) {
      if (remove.includes(j)) continue; // we already attached it to something else
      if (data[j].born < data[i].died) continue; // violates temporality
      if ((data[j].born - data[i].died) > 64) continue; // that's just silly. if it's been 2 seconds, they're probably not the same object.
      var pathJStart = data[j].path[0];
      if (nearby(pathIEnd,pathJStart)) {
        data[i].path = data[i].path.concat(data[j].path);
        data[i].died = data[j].died;
        console.log(data[i].label + "->" + data[j].label);
        remove.push(j);
        // note that we don't break the loop here. we can keep extending a single path.
      }
    }
  }
  console.log("removing these after splicing: " + remove);
  data = removeIdxsFromArray(remove, data);
  remove.length = 0;

  return data;
}

function filterShortLife(data) {
  var remove = [];

  // pull out all the really short ones... we measure "short" in frames; at 32 per second, short ain't so short.
  var shortLife = 32*6;
  // we could have done this alongside the other thing (at the end of each extending loop), but... that's just confusing to read.
  for(var i=0; i < data.length; i++) {
    var lifeLength = data[i].died - data[i].born;
    if (lifeLength <= shortLife) {
      remove.push(i);
      console.log(i + "(" + data[i].label + "):life X\t" + lifeLength);
    } else {
      console.log(i + "(" + data[i].label + "):life \t\t\t\t\tO\t" + lifeLength);
    }
  }
  console.log("removing these after short life filtering: " + remove);
  data = removeIdxsFromArray(remove, data);
  remove.length = 0;

  return data;
}

function filterSmallMotion(data) {
  var remove = [];

  // now pull out the things that don't move much over the course of their life.
  var sensibleLength = 400.0; // pixels
  for(var i=0; i < data.length; i++) {
    var lengthOfLine = lineLength(data[i].path);
    if (lengthOfLine < sensibleLength) {
      console.log(i + "(" + data[i].label + "):length X\t" + lengthOfLine);
      remove.push(i);
    } else {
      console.log(i + "(" + data[i].label + "):length\t\t\t\t\tO\t" + lengthOfLine);
    }
  }
  console.log("removing these after short travel filtering: " + remove);
  data = removeIdxsFromArray(remove, data);
  remove.length = 0;

  return data;
}

function filterData(data) {
  // TODO: this doesn't take into account the phenomenon of "2-1-2", where players' blobs can get joined together and then separate

  data = stringTogether(data);
  data = filterShortLife(data);
  data = filterSmallMotion(data);

  return data;
}
