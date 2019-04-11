var xaudiopro = require("./xaudiopro.js");
var fs = require("fs");

xaudiopro({
  // Mount /data inside application to the current directory.
  mounts: [{type: "NODEFS", opts: {root: "."}, mountpoint: "/data"}],
  //arguments: ["-i", "/data/hiss-one.wav", "-o", "/data/hiss-one-wasm.wav"],
  //arguments: ["-t", "0", "-i", "/data/hiss-speech.wav", "-o", "/data/hiss-speech-wasm.wav"],
  arguments: ["-t", "0", "-i", "/data/pm.wav", "-o", "/data/pm-wasm.wav"],
  stdin: function() {},
});


