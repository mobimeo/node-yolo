{
  "variables": {
    'with_opencv%': '<!(node ./has_lib.js opencv)',
    'with_cuda%': '<!(node ./has_lib.js cuda)'
  },
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
        "-lstdc++"
      ],
      "include_dirs": [
        "./src",
        "<!(node -e \"require('nan')\")",
      ],
      "cflags": [
        "-Wall",
        "-Wfatal-errors",
        "-fPIC",
        "-Ofast"
      ],
      "conditions": [
        ['with_opencv=="true"', {
          "defines": [
            "OPENCV",
          ],
          "libraries": [
            "-lopencv_core",
            "-lopencv_highgui"
          ]
        }],
        ['with_cuda=="true"', {
          "defines": [
            "GPU"
          ],
          "libraries": [
            "-lcuda",
            "-lcudart",
            "-lcublas",
            "-lcurand"
          ]
        }]
      ]
    }
  ]
}