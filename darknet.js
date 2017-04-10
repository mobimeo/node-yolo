var darknet = require('bindings')('darknet');

module.exports = {
  detectImage(cfg, cb) {
    cfg.thresh = cfg.thresh || 0.24;
    cfg.hierThresh = cfg.hierThresh || 0.5;
    darknet.detectImage(cfg, cb);
  },
  detect(cfg, cb) {
    const captureFromFile = !!cfg.video;
    const captureFromCam = !captureFromFile;
    const cameraIndex = cfg.cameraIndex || 0;
    const thresh = cfg.thresh || 0.24;
    const hierThresh = cfg.hierThresh || 0.5;
    const frameSkip = cfg.frameSkip || 0;
    darknet.detect({
      cfg: cfg.cfg,
      weights: cfg.weights,
      data: cfg.data,
      captureFromFile,
      video: cfg.video,
      captureFromCam,
      cameraIndex,
      thresh,
      hierThresh,
      frameSkip,
    }, cb);
  }
};