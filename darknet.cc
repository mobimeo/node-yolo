#include <nan.h>
#include "src/demo.h"

void detect(const Nan::FunctionCallbackInfo<v8::Value>& arguments) {
  v8::Local<v8::Object> options = arguments[0].As<v8::Object>();
  v8::Local<v8::Function> cb = arguments[1].As<v8::Function>();
  start_demo(options, cb);
}

void Init(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
  Nan::SetMethod(module, "exports", detect);
}

NODE_MODULE(addon, Init)