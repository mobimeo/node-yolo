var darknet = require('../darknet');
var fs = require('fs');
var i = 0;

darknet({
  cfgFile: './cfg/yolo.cfg',
  weightFile: './yolo.weights',
  dataFile: './cfg/coco.data',
  namesFile: './data/names.list',
}, function(modified, original, detections) {
  fs.writeFileSync(`modified${i}.jpeg`, modified);
  fs.writeFileSync(`original${i}.jpeg`, original);
  console.log('detections', detections);
  // [ { x: 0.6802602410316467,
  //     y: 0.340450644493103,
  //     w: 0.29339638352394104,
  //     h: 0.41068965196609497,
  //     prob: 0.6095095872879028,
  //     name: 'tvmonitor' },
  //   { x: 0.42017969489097595,
  //     y: 0.5601820349693298,
  //     w: 0.08811717480421066,
  //     h: 0.18914295732975006,
  //     prob: 0.2968880832195282,
  //     name: 'person' },
  //   { x: 0.042734742164611816,
  //     y: 0.6489028930664062,
  //     w: 0.09196034073829651,
  //     h: 0.3849184215068817,
  //     prob: 0.5312328934669495,
  //     name: 'person' },
  //   { x: 0.11737681925296783,
  //     y: 0.6608229279518127,
  //     w: 0.180913046002388,
  //     h: 0.25461822748184204,
  //     prob: 0.28867143392562866,
  //     name: 'diningtable' },
  //   { x: 0.18512842059135437,
  //     y: 0.6649541258811951,
  //     w: 0.15171007812023163,
  //     h: 0.23741166293621063,
  //     prob: 0.2520630657672882,
  //     name: 'diningtable' },
  //   { x: 0.5869240164756775,
  //     y: 0.6576676964759827,
  //     w: 0.736821174621582,
  //     h: 0.6883207559585571,
  //     prob: 0.5906478762626648,
  //     name: 'person' },
  //   { x: 0.8672126531600952,
  //     y: 0.8073385953903198,
  //     w: 0.1472286880016327,
  //     h: 0.270913690328598,
  //     prob: 0.2826858460903168,
  //     name: 'chair' } ]
  i++;
});