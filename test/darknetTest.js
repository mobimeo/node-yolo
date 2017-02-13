const darknet = require('../darknet');
const fs = require('fs');
const spawn = require('child_process').spawn;

let ffmpeg = null;

function getPipe() {
  if (!ffmpeg) {
    ffmpeg = spawn('ffmpeg',
      [ '-y',
        '-re',
        '-loglevel', 'verbose',
        // '-f', 'avfoundation',
        // '-r', '30',
        // '-i', '0',
        '-f', 'rawvideo',
        '-pix_fmt', 'bgr24',
        '-s', '1280x720',
        '-r', '1',
        '-i', '-',
        '-c:v', 'libx264',
        '-an',
        '-s', '1280x720',
        // 'http://localhost:8090/feed1.ffm'
        'output.mp4'
      ], {
        stdio: ['pipe',  process.stdout, process.stderr],
      });
    return ffmpeg.stdin;
  } else {
    return ffmpeg.stdin;
  }
}

try {
  fs.unlinkSync('data.raw');
} catch (err) {

}

let i = 0;
darknet({
  cfgFile: './cfg/yolo.cfg',
  weightFile: './yolo.weights',
  dataFile: './cfg/coco.data',
  namesFile: './data/coco.names',
}, function(modified, original, detections) {
  console.log('Got frame', modified);
  console.log(detections);
  const pipe = getPipe();
  fs.appendFileSync('data.raw', modified);
  pipe.write(modified);
  i++;
});

setInterval(function() {
  console.log('Async');
}, 1000);