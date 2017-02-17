const darknet = require('../darknet');
const fs = require('fs');

darknet.detectImage({
  cfg: './cfg/yolo.cfg',
  weights: './yolo.weights',
  data: './cfg/coco.data',
  image: './test.jpg',
}, function(modified, original, detections) {
  console.log(modified.length, original.length, detections);
  fs.writeFileSync('./data.modified.raw', modified);
  // ffmpeg -f rawvideo -s 768x576 -pix_fmt bgr24 -i data.raw data.jpg
  fs.writeFileSync('./data.raw', original);
  // ffmpeg -f rawvideo -s 768x576 -pix_fmt bgr24 -i data.modified.raw data.jpg
});