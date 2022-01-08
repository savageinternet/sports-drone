function getSharpTurnCount(path) {
  var sharpTurns = 0;
  var sharpThresh = 5 * Math.PI/180; // the acos returns in radians, so we convert degrees to radians here for our checks
  for (var i = 0; i < path.length - 3; i++) {
    // we're using the law of cosines: c^2 = a^2 + b^2 + 2*a*b*cos(C)
    // a.k.a. C = arccos((a^2+b^2-c^2)/(2*a*b))
    var pt0 = path[i];
    var pt1 = path[i+1];
    var pt2 = path[i+2];
    var a = distance(pt0,pt1);
    var b = distance(pt1,pt2);
    var c = distance(pt0,pt2);
    if (a + b > c) {
      var angle = Math.acos((a*a+b*b-c*c)/(2*a*b));
      if (angle < sharpThresh) sharpTurns++;
    }
  }
  return sharpTurns;
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

function filterNonHumanLike(data) {
  var remove = [];

  // human-like motion means that the blobs move... like humans. how do we quantify that? well, for now we'll look at the number of <90 degree turns made by a blob.
  var okNumOfSharpTurnsPerSec = 0.2;
  var roughFramesPerSec = 32; // I mean... roughly. should work with 60fps footage, too, though.
  for(var i=0; i < data.length; i++) {
    var checking = data[i];
    var sharpTurns = getSharpTurnCount(checking.path);
    var lifeInSec = (checking.died-checking.born)/roughFramesPerSec;
    if (sharpTurns/lifeInSec > okNumOfSharpTurnsPerSec) {
      console.log(i + "(" + checking.label + "):sharpTurnsPerSec X\t" + sharpTurns/lifeInSec);
      remove.push(i);
    } else {
      console.log(i + "(" + checking.label + "):sharpTurnsPerSec\t\t\t\t\tO\t" + sharpTurns/lifeInSec);
    }
  }

  console.log("removing these after non-human movement filtering: " + remove);
  data = removeIdxsFromArray(remove, data);
  remove.length = 0;

  return data;
}

function filterData(data) {
  // TODO: this doesn't take into account the phenomenon of "2-1-2", where players' blobs can get joined together and then separate

  data = stringTogether(data);
  data = filterShortLife(data);
  data = filterSmallMotion(data);
  data = filterNonHumanLike(data);
  // TODO: need to protect data that has a positive code-based ID at some point on it. the non-human-like filter here is actually filtering out the goalies (lol), and that ain't no good to nobody. if we can ID them, we can do mo' betta.

  return data;
}
