var wid = 1280;
var ht = 720;

var mData;

// stuff
var svg = d3.select('body')
    .append('svg')        // create an <svg> element
    .attr('width', wid) // set its dimentions
    .attr('height', ht);

d3.json('/data.json')
  .then(function(data) {
    mData = JSON.parse(JSON.stringify(data));
    data['tracked'] = filterData(data['tracked']);
    doDrawing(data);
  });

function doDrawing(data) {
  svg.selectAll("*").remove();

  // define the scales that we use to map our x y things into screen space
  var x = d3.scaleLinear()
    .range([0, wid])
    .domain([0, data['width']]);
  var y = d3.scaleLinear()
    .range([0, ht])
    .domain([data['height'], 0]);

  // TODO would be smart to draw in the field here... need to probably get some data on this from the video first, tho. still, Imma just cheat a lil here... :O
  // this draws a fakey soccer field
  svg.append("rect")
      .attr("x", 5)
      .attr("y", 5)
      .attr("width", wid-10)
      .attr("height", ht-10)
      .style("stroke", "black")
      .style("stroke-width", 3)
      .attr("fill", "none");
  // goalie boxes
  svg.append("rect")
      .attr("x", 5)
      .attr("y", (ht-300)/2+5)
      .attr("width", 200)
      .attr("height", 300)
      .style("stroke", "black")
      .style("stroke-width", 3)
      .attr("fill", "none");
  svg.append("rect")
      .attr("x", wid-5-200)
      .attr("y", (ht-300)/2+5)
      .attr("width", 200)
      .attr("height", 300)
      .style("stroke", "black")
      .style("stroke-width", 3)
      .attr("fill", "none");
  // half
  svg.append("rect")
      .attr("x", 5)
      .attr("y", 5)
      .attr("width", (wid-10)/2)
      .attr("height", ht-10)
      .style("stroke", "black")
      .style("stroke-width", 3)
      .attr("fill", "none");

  // draw a line for each labelled thingy
  var linez = svg.selectAll("path").data(data['tracked']);
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
    .attr("stroke", function() {
        return "hsl(" + Math.random() * 360 + ",100%,50%)";
    }) // make it a random color
    ;

  // draw a label at the end of each labelled thingy's line
  var textz = svg.selectAll("text").data(data['tracked']);
  var newTextz = textz.enter();
  newTextz.append('text')
    .attr('x', function(labelled) {
      return x(labelled.path[labelled.path.length - 1].x);
    })
    .attr('y', function(labelled) {
      return y(labelled.path[labelled.path.length - 1].y);
    })
    .attr("text-anchor", "middle")
    .text(function(labelled) { return labelled.label; });

  // draw a dot at the beginning of each labelled thingy's line
  var circlez = svg.selectAll("circle").data(data['tracked']);
  var newCirclez = circlez.enter();
  newCirclez.append('circle')
    .attr('x', function(labelled) {
      return x(labelled.path[0].x);
    })
    .attr('y', function(labelled) {
      return y(labelled.path[0].y);
    })
    .attr('r', 2)
    .style('fill', 'black');

  // title it
  svg.append("text")
      .attr("x", (wid / 2))
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
  var shortLife = 32*3;
  // we could have done this alongside the other thing (at the end of each extending loop), but... that's just confusing to read.
  for(var i=0; i < data.length; i++) {
    var lifeLength = data[i].died - data[i].born;
    if (lifeLength <= shortLife) {
      remove.push(i);
      console.log(i + ":life X " + data[i].label + "\t" + lifeLength);
    } else {
      console.log(i + ":life \t\t\t\t\tO " + data[i].label + "\t" + lifeLength);
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
  var sensibleLength = 100.0; // pixels
  for(var i=0; i < data.length; i++) {
    var lengthOfLine = lineLength(data[i].path);
    if (lengthOfLine < sensibleLength) {
      console.log(i + ":length X " + data[i].label + "\t" + lengthOfLine);
      remove.push(i);
    } else {
      console.log(i + ":length\t\t\t\t\tO " + data[i].label + "\t" + lengthOfLine);
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
