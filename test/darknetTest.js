const darknet = require('../darknet');
const fs = require('fs');
const spawn = require('child_process').spawn;
let ffmpeg = null;

function getPipe(dimensions) {
  if (!ffmpeg) {
    ffmpeg = spawn('ffmpeg',
      [ '-y',
        '-loglevel', 'verbose',
        '-re',
        '-f', 'rawvideo',
        '-pix_fmt', 'rgb24',
        '-s', `${dimensions.width}x${dimensions.height}`,
        '-i', '-',
        '-r', '10',
        'detected.mp4'
      ], {
        stdio: ['pipe', process.stdout, process.stderr],
      });
    return ffmpeg.stdin;
  } else {
    return ffmpeg.stdin;
  }
}

darknet.detect({
  cfg: './cfg/yolo.cfg',
  weights: './yolo.weights',
  data: './cfg/coco.data',
  cameraIndex: 0,
  frameSkip: 0, // how many frames to skip, when calling the callback
}, function(modified, original, detections, dimensions) {
  getPipe(dimensions).write(modified);
});
