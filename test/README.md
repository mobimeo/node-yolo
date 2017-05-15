You already have the config and data file for YOLO in the `test/cfg` and `test/data` subfolders. You will have to download the pre-trained weight file [here (258 MB)](http://pjreddie.com/media/files/yolo.weights). Or just run this:

```sh
cd node-yolo/test
wget http://pjreddie.com/media/files/yolo.weights
```

All config and data files courtesy of https://github.com/pjreddie/darknet

Note: you will need `ffmpeg` to run the `darknetTest.js`

## Run the examples

To test with your webcam:

```
node darknetTest.js
```

To test on an image:

```
node darknetImageTest.js
```
