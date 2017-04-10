#ifndef DEMO_TYPES
#define DEMO_TYPES

struct RecognitionResult {
   float x;
   float y;
   float w;
   float h;
   float prob;
   char* name;
};

struct WorkerData {
  char * originalFrame;
  char * modifiedFrame;
  int originalFrameSize;
  int modifiedFrameSize;
  RecognitionResult* recognitionResults;
  int numberOfResults;
  int frameWidth;
  int frameHeight;
};

struct InputOptions {
  char cfgfile[512];
  char weightfile[512];
  char datafile[512];
  int cameraIndex;
  char videofile[512];
  char imagefile[512];
  int captureFromCamera;
  int captureFromFile;
  float thresh;
  float hierThresh;
  int frameSkip;
};

#endif