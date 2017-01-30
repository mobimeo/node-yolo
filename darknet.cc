#include <nan.h>
#include "src/demo.h"

void RunCallback(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  v8::Local<v8::Function> cb = info[1].As<v8::Function>();
  v8::Local<v8::Object> options = info[0].As<v8::Object>();
  start_demo(options, cb);
}

void Init(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
  Nan::SetMethod(module, "exports", RunCallback);
}

NODE_MODULE(addon, Init)