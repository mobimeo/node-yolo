{
  "targets": [
    {
      "target_name": "darknet",
      "sources": [ "darknet.cc", "src/demo.cc"],
      "libraries": [ "-ldarknet", "-lopencv_core", "-lopencv_highgui" ],
      "include_dirs": [
        "./src",
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}