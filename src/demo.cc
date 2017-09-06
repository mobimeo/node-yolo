#include <nan.h>
#include <v8.h>
#include "demo.h"
#include <vector>

#include <sys/time.h>
#include "darknet/network.h"
#include "darknet/detection_layer.h"
#include "darknet/region_layer.h"
#include "darknet/cost_layer.h"
#include "darknet/utils.h"
#include "darknet/parser.h"
#include "darknet/box.h"
#include "darknet/image.h"
#include "darknet/demo.h"
#include "darknet/option_list.h"
#include "darknet/data.h"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"

extern "C" {
    image get_image_from_stream(CvCapture *cap);
}

using v8::Local;
using v8::Function;
using v8::Value;

#define FRAMES 3

static char **demo_names;
static image **demo_alphabet;
static int demo_classes;

static float **probs;
static box *boxes;
static network net;
static image in   ;
static image in_s ;
static image det  ;
static image det_s;
static CvCapture * cap;
static float demo_thresh = 0.24;
static float demo_hier_thresh = .5;

static float *predictions[FRAMES];
static int demo_index = 0;
static image images[FRAMES];
static float *avg;
static image modified_frame;
static image original_frame;

std::vector<RecognitionResult> current_results;

void *fetch_in_thread(void *ptr) {
    in = get_image_from_stream(cap);
    if(!in.data){
        error("Stream closed.");
    }
    in_s = resize_image(in, net.w, net.h);
    return 0;
}

void capture_detections(int num, float thresh, box *boxes, float **probs, char **names, image **alphabet, int classes) {
    int i;
    current_results.clear();
    for (i = 0; i < num; ++i) {
        int cl = max_index(probs[i], classes);
        float prob = probs[i][cl];
        if (prob > thresh) {
            box b = boxes[i];
            current_results.push_back({
                .x = b.x,
                .y = b.y,
                .w = b.w,
                .h = b.h,
                .prob = prob,
                .name = names[cl]
            });
        }
    }
}

void *detect_in_thread(void *ptr) {
    float nms = .4;

    layer l = net.layers[net.n-1];
    float *X = det_s.data;
    float *prediction = network_predict(net, X);

    memcpy(predictions[demo_index], prediction, l.outputs*sizeof(float));
    mean_arrays(predictions, FRAMES, l.outputs, avg);
    l.output = avg;

    free_image(det_s);
    if(l.type == DETECTION){
        get_detection_boxes(l, 1, 1, demo_thresh, probs, boxes, 0);
    } else if (l.type == REGION){
        get_region_boxes(l, 1, 1, net.w, net.h, demo_thresh, probs, boxes, 0, 0, 0, demo_hier_thresh, 1);
    } else {
        error("Last layer must produce detections\n");
    }
    if (nms > 0) do_nms(boxes, probs, l.w*l.h*l.n, l.classes, nms);

    free_image(images[demo_index]);

    images[demo_index] = det;
    det = images[(demo_index + FRAMES/2 + 1)%FRAMES];
    demo_index = (demo_index + 1)%FRAMES;

    original_frame = copy_image(det);
    draw_detections(det, l.w*l.h*l.n, demo_thresh, boxes, probs, 0, demo_names, demo_alphabet, demo_classes);
    capture_detections(l.w*l.h*l.n, demo_thresh, boxes, probs, demo_names, demo_alphabet, demo_classes);
    modified_frame = copy_image(det);

    return 0;
}

IplImage* createIplImage(image input) {
    int x,y,k;

    IplImage *disp = cvCreateImage(cvSize(input.w,input.h), IPL_DEPTH_8U, input.c);
    int step = disp->widthStep;

    for(y = 0; y < input.h; ++y){
        for(x = 0; x < input.w; ++x){
            for(k= 0; k < input.c; ++k){
                disp->imageData[y*step + x*input.c + k] = (unsigned char)(get_pixel(input,x,y,k)*255);
            }
        }
    }

    return disp;
}

void callback(const typename Nan::AsyncProgressWorkerBase<WorkerData>::ExecutionProgress& progress) {
    image modified = modified_frame;
    image original = original_frame;
    image modified_copy = copy_image(modified);
    image original_copy = copy_image(original);
    if(modified.c == 3) rgbgr_image(modified_copy);
    if(original.c == 3) rgbgr_image(original_copy);

    IplImage *disp = createIplImage(modified);
    char* rawData;
    rawData = (char *) malloc(disp->imageSize);
    memcpy(rawData, disp->imageData, disp->imageSize);

    IplImage *dispOriginal = createIplImage(original);

    char* rawDataOriginal;
    rawDataOriginal = (char *) malloc(dispOriginal->imageSize);
    memcpy(rawDataOriginal, dispOriginal->imageData, dispOriginal->imageSize);

    WorkerData progressState;

    progressState.frameWidth = original.w;
    progressState.frameHeight = original.h;

    progressState.modifiedFrame = rawData;
    progressState.modifiedFrameSize = disp->imageSize;

    progressState.originalFrame = rawDataOriginal;
    progressState.originalFrameSize = dispOriginal->imageSize;

    progressState.numberOfResults = current_results.size();
    progressState.recognitionResults = new RecognitionResult[progressState.numberOfResults];

    int i = 0;
    for(auto const& result: current_results) {
        progressState.recognitionResults[i] = result;
        i++;
    }

    progress.Send(&progressState, sizeof( progressState ));

    cvReleaseImage(&disp);
    free_image(modified_copy);

    cvReleaseImage(&dispOriginal);
    free_image(original_copy);

    free_image(modified_frame);
    free_image(original_frame);

}

void start_demo(InputOptions opts, const typename Nan::AsyncProgressWorkerBase<WorkerData>::ExecutionProgress& progress) {
    list *options = read_data_cfg(opts.datafile);
    char classesParam[] = "classes";
    char namesParam[] = "names";
    char defaultNamesList[] = "data/names.list";
    int classes = option_find_int(options, classesParam, 20);
    char *name_list = option_find_str(options, namesParam, defaultNamesList);
    char **names = get_labels(name_list);
    int cam_index = opts.cameraIndex;
    int delay = opts.frameSkip;
    demo_thresh = opts.thresh;
    demo_hier_thresh = opts.hierThresh;
    image **alphabet = load_alphabet();
    demo_names = names;
    demo_alphabet = alphabet;
    demo_classes = classes;
    net = parse_network_cfg(opts.cfgfile);
    load_weights(&net, opts.weightfile);
    set_batch_network(&net, 1);

    srand(2222222);

    if (opts.captureFromFile) {
        cap = cvCaptureFromFile(opts.videofile);
    } else {
        cap = cvCaptureFromCAM(cam_index);
    }

    if(!cap) error("Couldn't connect to webcam.\n");

    layer l = net.layers[net.n-1];
    int j;

    avg = (float *) calloc(l.outputs, sizeof(float));
    for(j = 0; j < FRAMES; ++j) predictions[j] = (float *) calloc(l.outputs, sizeof(float));
    for(j = 0; j < FRAMES; ++j) {
        images[j] = make_image(1,1,3);
    }

    boxes = (box *)calloc(l.w*l.h*l.n, sizeof(box));
    probs = (float **)calloc(l.w*l.h*l.n, sizeof(float *));
    for(j = 0; j < l.w*l.h*l.n; ++j) probs[j] = (float *)calloc(l.classes, sizeof(float *));


    printf("Started\n");

    pthread_t fetch_thread;
    pthread_t detect_thread;

    fetch_in_thread(0);
    det = in;
    det_s = in_s;

    fetch_in_thread(0);
    detect_in_thread(0);
    det = in;
    det_s = in_s;

    for(j = 0; j < FRAMES/2; ++j){
        fetch_in_thread(0);
        detect_in_thread(0);
        det = in;
        det_s = in_s;
    }

    while(1) {
        if(pthread_create(&fetch_thread, 0, fetch_in_thread, 0)) error("Thread creation failed");
        if(pthread_create(&detect_thread, 0, detect_in_thread, 0)) error("Thread creation failed");

        pthread_join(fetch_thread, 0);
        pthread_join(detect_thread, 0);

        if (delay == 0) {
            callback(progress);
        }


        det   = in;
        det_s = in_s;

        --delay;
        if(delay < 0){
            delay = opts.frameSkip;
        }
    }
}

WorkerData* start_image_demo(InputOptions opts) {
    list *options = read_data_cfg(opts.datafile);
    char classesParam[] = "classes";
    char namesParam[] = "names";
    char defaultNamesList[] = "data/names.list";
    int classes = option_find_int(options, classesParam, 20);
    char *name_list = option_find_str(options, namesParam, defaultNamesList);
    char **names = get_labels(name_list);
    int j;
    float thresh = opts.thresh;
    float hier_thresh = opts.hierThresh;

    image **alphabet = load_alphabet();
    demo_names = names;
    demo_alphabet = alphabet;
    demo_classes = classes;
    net = parse_network_cfg(opts.cfgfile);
    load_weights(&net, opts.weightfile);
    set_batch_network(&net, 1);

    char * input = opts.imagefile;
    float nms=.4;

    image im = load_image_color(input,0,0);
    image sized = resize_image(im, net.w, net.h);
    layer l = net.layers[net.n-1];

    boxes = (box *)calloc(l.w*l.h*l.n, sizeof(box));
    probs = (float **)calloc(l.w*l.h*l.n, sizeof(float *));
    for(j = 0; j < l.w*l.h*l.n; ++j) probs[j] = (float *)calloc(l.classes, sizeof(float *));

    float *X = sized.data;
    network_predict(net, X);
    if(l.type == DETECTION){
        printf("detection \n");
    } else if (l.type == REGION){
        printf("region \n");
    } else {
        printf("error: Last layer must produce detections\n");
    }
    get_region_boxes(l, 1, 1, net.w, net.h, demo_thresh, probs, boxes, 0, 0, 0, hier_thresh, 1);
    if (l.softmax_tree && nms) do_nms_obj(boxes, probs, l.w*l.h*l.n, l.classes, nms);
    else if (nms) do_nms_sort(boxes, probs, l.w*l.h*l.n, l.classes, nms);

    image original = im;
    image original_copy = copy_image(original);

    draw_detections(im, l.w*l.h*l.n, thresh, boxes, probs, 0, names, alphabet, l.classes);
    capture_detections(l.w*l.h*l.n, demo_thresh, boxes, probs, demo_names, demo_alphabet, demo_classes);

    image modified = im;
    image modified_copy = copy_image(modified);

    if(modified_copy.c == 3) rgbgr_image(modified_copy);
    if(original_copy.c == 3) rgbgr_image(original_copy);

    IplImage *disp = createIplImage(modified_copy);
    char* rawData;
    rawData = (char *) malloc(disp->imageSize);
    memcpy(rawData, disp->imageData, disp->imageSize);

    IplImage *dispOriginal = createIplImage(original_copy);
    char* rawDataOriginal;
    rawDataOriginal = (char *) malloc(dispOriginal->imageSize);
    memcpy(rawDataOriginal, dispOriginal->imageData, dispOriginal->imageSize);

    WorkerData* progressState = (WorkerData* ) malloc(sizeof(WorkerData));

    progressState->modifiedFrame = rawData;
    progressState->modifiedFrameSize = disp->imageSize;

    progressState->originalFrame = rawDataOriginal;
    progressState->originalFrameSize = dispOriginal->imageSize;

    progressState->numberOfResults = current_results.size();
    progressState->recognitionResults = new RecognitionResult[progressState->numberOfResults];

    progressState->frameWidth = original.w;
    progressState->frameHeight = original.h;

    int i = 0;
    for(auto const& result: current_results) {
        progressState->recognitionResults[i] = result;
        i++;
    }

    free_image(im);
    free_image(sized);
    free_image(original_copy);
    free_image(modified_copy);
    free(boxes);
    free_ptrs((void **)probs, l.w*l.h*l.n);

    return progressState;
}
