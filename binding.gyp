{
  "targets": [
    {
      "target_name": "darknet",
      "sources": [
        "darknet.cc",
        "src/demo.cc"
      ],
      "libraries": [
        "-lm",
        "-pthread",
        "-ldarknet",
        "-lopencv_core",
        "-lopencv_highgui",
        "-L/usr/local/cuda/lib64",
        "-L/usr/local/cuda/lib",
        "-lcuda",
        "-lcudart",
        "-lcublas",
        "-lcurand",
        "-lstdc++"
      ],
      "include_dirs": [
        "./src",
        "<!(node -e \"require('nan')\")"
      ],
      "defines": [
        "OPENCV",
        "GPU"
      ],
      "cflags": [
        "-Wall",
        "-Wfatal-errors",
        "-fPIC",
        "-Ofast"
      ]
    }
  ]
}