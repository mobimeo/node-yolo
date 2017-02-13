#include <nan.h>
#include "src/demo.h"
#include <unistd.h>
#include "src/types.h"

using namespace Nan;


template<typename T>
class DetectionWorker : public AsyncProgressWorkerBase<T> {
 public:
  DetectionWorker(Callback *callback, Callback *progress, InputOptions opts)
    : AsyncProgressWorkerBase<T>(callback), progressCb(progress), opts(opts) {}
  ~DetectionWorker() {}

  void Execute(const typename AsyncProgressWorkerBase<T>::ExecutionProgress& progress) {
    start_demo(opts, progress);
  }

  void HandleProgressCallback(const T *data, size_t size) {
    HandleScope scope;
    const int argc = 3;
    v8::Local<v8::Array> results = Nan::New<v8::Array>();
    printf("Predictions size %d \n", data->numberOfResults);
    for(int i = 0; i < data->numberOfResults; i++) {
        RecognitionResult result = data->recognitionResults[i];
        v8::Local<v8::Object> obj = Nan::New<v8::Object>();
        obj->Set(Nan::New("x").ToLocalChecked(), Nan::New<v8::Number>(result.x));
        obj->Set(Nan::New("y").ToLocalChecked(), Nan::New<v8::Number>(result.y));
        obj->Set(Nan::New("w").ToLocalChecked(), Nan::New<v8::Number>(result.w));
        obj->Set(Nan::New("h").ToLocalChecked(), Nan::New<v8::Number>(result.h));
        obj->Set(Nan::New("prob").ToLocalChecked(), Nan::New<v8::Number>(result.prob));
        obj->Set(Nan::New("name").ToLocalChecked(), Nan::New<v8::String>(result.name).ToLocalChecked());
        results->Set(i, obj);
        i++;
    }

    v8::Local<v8::Value> argv[argc] = {
        Nan::NewBuffer(data->modifiedFrame, data->modifiedFrameSize).ToLocalChecked(),
        Nan::NewBuffer(data->originalFrame, data->originalFrameSize).ToLocalChecked(),
        results,
    };

    progressCb->Call(argc, argv);
  }

  private:
    Callback* progressCb;
    InputOptions opts;
};

void detect(const Nan::FunctionCallbackInfo<v8::Value>& arguments) {
  v8::Local<v8::Object> opts = arguments[0].As<v8::Object>();

  v8::Local<v8::Function> function;
  Nan::Callback callback(function);
  Nan::Callback *progress = new Nan::Callback(arguments[1].As<Function>());

  v8::Local<v8::Value> cfgFile = Nan::Get(opts, Nan::New("cfgFile").ToLocalChecked()).ToLocalChecked();
  v8::String::Utf8Value cfgFileStr(cfgFile);
  char* cfgfile = *cfgFileStr;

  v8::Local<v8::Value> weightFile = Nan::Get(opts, Nan::New("weightFile").ToLocalChecked()).ToLocalChecked();
  v8::String::Utf8Value weightFileStr(weightFile);
  char* weightfile = *weightFileStr;

  v8::Local<v8::Value> dataFile = Nan::Get(opts, Nan::New("dataFile").ToLocalChecked()).ToLocalChecked();
  v8::String::Utf8Value dataFileStr(dataFile);
  char* datafile = *dataFileStr;

  v8::Local<v8::Value> namesFile = Nan::Get(opts, Nan::New("namesFile").ToLocalChecked()).ToLocalChecked();
  v8::String::Utf8Value namesFileStr(namesFile);
  char* namesfile = *namesFileStr;

  InputOptions inputOptions;
  strcpy(inputOptions.cfgfile, cfgfile);
  strcpy(inputOptions.weightfile, weightfile);
  strcpy(inputOptions.datafile, datafile);
  strcpy(inputOptions.namesfile, namesfile);
  Nan::AsyncQueueWorker(new DetectionWorker<WorkerData>(&callback, progress, inputOptions));
}

void Init(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
  Nan::SetMethod(module, "exports", detect);
}

NODE_MODULE(addon, Init)