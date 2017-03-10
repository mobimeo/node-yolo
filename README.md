# Node.js Wrapper for YOLO/Darknet recognition framework

Darknet is [an open source neural network framework written in C and CUDA](https://github.com/pjreddie/darknet).
This project wraps this framework in a Node.js native addon.

- [darknet](http://pjreddie.com/darknet/)
- [YOLO (You only look once)](http://pjreddie.com/darknet/yolo/)

## Status

Currently, the wrapper implements a demo method to run a recognition from the webcam/video or an image file. Equivalent to

```sh
./darknet detector demo cfg/coco.data cfg/yolo.cfg yolo.weights
./darknet detect cfg/yolo.cfg yolo.weights data/dog.jpg
```

## Prerequisites

This module requires OpenCV installed. First, you need to compile [this fork](https://github.com/OrKoN/darknet) of darknet with OpenCV support (optionally, with CUDA support):

```sh
git clone git@github.com:OrKoN/darknet.git
cd darknet
make OPENCV=1 # optionally GPU=1
make install # by default installed to /usr/local
```

After that you can process with the installation via NPM.

The fork is required because it contains [a few important changes](https://github.com/pjreddie/darknet/compare/master...OrKoN:master) to the origin source code which allow using darknet with NodeJS:

- Makefile is extended to build a static library (darknet.a).
- Makefile is extended with `install` and `uninstall` commands which install the library globally so that this module can easily find and link it.
- All darknet functions in header files are marked with `extern "C"` if they are included in a C++ program (such as a NodeJS module).

## Installation

```sh
npm install @moovel/yolo --save
```

## Usage

Either download you own `cfg`, `data` folders and `.weight` files from the darknet project or use the ones included in `test` folder (see also [test/readme.md](test/README.md)). You also can find there the examples from below.

Detect a video from camera or a file:

```js
const darknet = require('@moovel/yolo');

darknet.detect({
  cfg: './cfg/yolo.cfg',
  weights: './yolo.weights',
  data: './cfg/coco.data',
  cameraIndex: 0, // optional, default: 0,
  video: "./test.mp4", // optional, forces to use the video file instead of a camera
  thresh: 0.24,
  hierThresh: 0.5,
}, function(modified, original, detections) {
  /**

  modified - raw frame with detections drawn, rgb24 format
  original - raw frame, as captured by the webcam/video, rgb24 format,
  detections - array of detections

  Example detections:

  [ { x: 0.8602103590965271,
      y: 0.20008485019207,
      w: 0.13895535469055176,
      h: 0.39782464504241943,
      prob: 0.2408987432718277,
      name: 'tvmonitor' },
    ,
    { x: 0.26072466373443604,
      y: 0.4977818727493286,
      w: 0.10842404514551163,
      h: 0.22796104848384857,
      prob: 0.3290732204914093,
      name: 'person' },
    ,
    { x: 0.2568981349468231,
      y: 0.5765896439552307,
      w: 0.12322483211755753,
      h: 0.2544059157371521,
      prob: 0.2738085687160492,
      name: 'chair' },
    ,
    { x: 0.6593853235244751,
      y: 0.8188746571540833,
      w: 0.06210440397262573,
      h: 0.100614033639431,
      prob: 0.3225017189979553,
      name: 'clock' } ]

  */
});
```

Detect on a single image:

```js
const darknet = require('@moovel/yolo');

darknet.detectImage({
  cfg: './cfg/yolo.cfg',
  weights: './yolo.weights',
  data: './cfg/coco.data',
  image: './data/dog.jpg',
  thresh: 0.24,
  hierThresh: 0.5,
}, function(modified, original, detections, dimensions) {
  /**

  modified - raw frame with detections drawn, rgb24 format
  original - raw frame, as captured by the webcam/video, rgb24 format,
  detections - array of detections
  dimenstions - image dimensions

  Example detections:

  [ { x: 0.8602103590965271,
      y: 0.20008485019207,
      w: 0.13895535469055176,
      h: 0.39782464504241943,
      prob: 0.2408987432718277,
      name: 'tvmonitor' },
    ,
    { x: 0.26072466373443604,
      y: 0.4977818727493286,
      w: 0.10842404514551163,
      h: 0.22796104848384857,
      prob: 0.3290732204914093,
      name: 'person' },
    ,
    { x: 0.2568981349468231,
      y: 0.5765896439552307,
      w: 0.12322483211755753,
      h: 0.2544059157371521,
      prob: 0.2738085687160492,
      name: 'chair' },
    ,
    { x: 0.6593853235244751,
      y: 0.8188746571540833,
      w: 0.06210440397262573,
      h: 0.100614033639431,
      prob: 0.3225017189979553,
      name: 'clock' } ]
  */
});
```

After that you can compress original frames with ffmpeg:

```sh
ffmpeg -f rawvideo -s 768x576 -pix_fmt rgb24 -i data.raw data.jpg
```

