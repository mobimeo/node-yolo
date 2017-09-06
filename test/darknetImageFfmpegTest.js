const darknet = require('@moovel/yolo');
const spawn = require('child_process').spawn;
let ffmpeg = null;

// ffmpeg -f rawvideo -s 768x576 -pix_fmt bgr24 -i data.modified.raw data.png
function ffmpegPipe(dimensions, filename) {
  if (!ffmpeg) {
    ffmpeg = spawn('ffmpeg',
      [
        '-loglevel', 'warning',
        '-f', 'rawvideo',
        '-pix_fmt', 'bgr24', // or 'rgb24' if color channels are swapped
        '-s', `${dimensions.width}x${dimensions.height}`,
        '-y',
        '-i', '-',
        filename
      ], {
        stdio: ['pipe', process.stdout, process.stderr],
      });
    return ffmpeg.stdin;
  } else {
    return ffmpeg.stdin;
  }
}

darknet.detectImage({
  cfg: './cfg/yolo.cfg',
  weights: './yolo.weights',
  data: './cfg/coco.data',
  image: './data/dog.jpg',
}, function(modified, original, detections, dimensions) {
  ffmpegPipe(dimensions, 'detected.png').write(modified, function() {
    process.exit(0);
  });
});
