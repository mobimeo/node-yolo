var darknet = require('bindings')('darknet');

module.exports = {
  detectImage(cfg, cb) {
    darknet.detectImage(cfg, cb);
  },
  detect(cfg, cb) {
    const captureFromFile = !!cfg.video;
    const captureFromCam = !captureFromFile;
    const cameraIndex = cfg.cameraIndex || 0;
    darknet.detect({
      cfg: cfg.cfg,
      weights: cfg.weights,
      data: cfg.data,
      captureFromFile,
      video: cfg.video,
      captureFromCam,
      cameraIndex,
    }, cb);
  }
};