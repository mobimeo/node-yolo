#ifndef NODE_DEMO
#define NODE_DEMO

#include <v8.h>
#include "types.h"

using v8::Local;
using v8::Function;

void start_demo(InputOptions options, const typename Nan::AsyncProgressWorkerBase<WorkerData>::ExecutionProgress& progress);
WorkerData* start_image_demo(InputOptions options);

#endif
