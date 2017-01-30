{
  "targets": [
    {
      "target_name": "darknet",
      "sources": [ "darknet.cc", "src/demo.cpp" ],
      "libraries": [ "/Users/rudenol/moovel/others/darknet/darknet.a", "-lopencv_core", "-lopencv_highgui" ],
      "include_dirs": [
        "/Users/rudenol/moovel/others/darknet/src",
        "/Users/rudenol/moovel/others/node-nan-darknet/src",
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}