#ifndef NODE_DEMO
#define NODE_DEMO

#include <v8.h>

using v8::Local;
using v8::Function;

void start_demo(v8::Local<v8::Object> options, Local<Function> cb);

#endif
