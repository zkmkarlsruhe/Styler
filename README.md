Styler
======

Arbitrary style transformation of an input image, video, or camera source.

This code base has been developed by [ZKM | Hertz-Lab](https://zkm.de/en/about-the-zkm/organization/hertz-lab) as part of the project [»The Intelligent Museum«](#the-intelligent-museum). 

Please raise issues, ask questions, throw in ideas or submit code, as this repository is intended to be an open platform to collaboratively improve language identification.

Copyright (c) 2022 ZKM | Karlsruhe.   
Copyright (c) 2022 Dan Wilcox.  

GPL v3 License.

Parts adapted from ofxTensorFlow2 example_style_transfer_arbitrary under a BSD Simplified License.

Dependencies
------------

* [openFrameworks](https://openframeworks.cc/download/)
* openFrameworks addons:
  - [ofxTensorFlow2](https://github.com/zkmkarlsruhe/ofxTensorFlow2)

Installation & Build
--------------------

Overview:

1. Follow the steps in the ofxTensorFlow2 "Installation & Build" section for you platform
2. Generate the project files for this folder using the OF ProjectGenerator
3. Build for your platform

Download the required TensorFlow 2 style transfer model using the provided script:

~~~shell
./scripts/
~~~

### Generating Project Files

Project files are not included so you will need to generate the project files for your operating system and development environment using the OF ProjectGenerator which is included with the openFrameworks distribution.

To (re)generate project files for an existing project:

1. Click the "Import" button in the ProjectGenerator
2. Navigate to the project's parent folder ie. "apps/myApps", select the base folder for the example project ie. "Styler", and click the Open button
3. Click the "Update" button

If everything went Ok, you should now be able to open the generated project and build/run the example.

### macOS

On macOS, a couple of additional manual steps are required to use ofxTensorflow2:

1. Enable C++14 in openFrameworks (only once, Xcode + Makefile)
2. Invoke `macos_install_libs.sh` in the Xcode project's Run Script build phases (after every project regeneration, Xcode only)

See the detailed steps in the [ofxTensorflow2 readme](https://github.com/zkmkarlsruhe/ofxTensorFlow2#macos).

For an Xcode build, open the Xcode project, select the "Styler Debug" scheme, and hit "Run".

For a Makefile build, build and run on the terminal:

```shell
cd Styler
make ReleaseTF2
make RunRelease
```
### Linux

Build and run on the terminal:

```shell
cd Styler
make Release
make RunReleaseTF2
```

Usage
-----

Styler applies a given style image onto an input image. The input image can come from one of three input sources: static image(s), video frames, or camera frames.

Styler starts in fullscreen and uses the camera source by default.

### Input

Styler lists image and video paths automatically on start from the following directories:
* `bin/data/style`: style images, jpg or png (required)
* `bin/data/image`: input images, jpg or png, all paths added to playlist
* `bin/data/video`: input video files, mov or mp4 or avi, first path used only

Simply add/remove files from each and restart the application. Order is sorted by filename.

_Note: a minimum of 1 image must be in the style directory, otherwise Styler will exit on start due to missing input. If the input image or video directories are empty, the respective source will be disabled._

While running, any images drag & dropped onto the Styler window will be loaded as a new style.

### Key Commands

* `v`: video input
* `c`: camera input
* `i`: image input
* `m`: mirror camera
* `n`: flip camera
* `r`: restart video
* `f`: toggle fullscreen
* `s`: save output image to `bin/data/output` directory
* `k`: toggle style input mode
* `a`: toggle auto style change after last frame
* `LEFT`: previous style
* `RIGHT`: next style
* `SPACE`: toggle playback / take style image
* `UP`: next frame, when paused
* `DOWN`: previous frame, when paused

### Automatic Style Change

When enabled, automatic style change will go to the next style based on the input source:
* camera: every 20 seconds
* image: when changing from the last iamge to the first image
* video: when changing from the last frame to the first frame

The Intelligent Museum
----------------------

An artistic-curatorial field of experimentation for deep learning and visitor participation

The [ZKM | Center for Art and Media](https://zkm.de/en) and the [Deutsches Museum Nuremberg](https://www.deutsches-museum.de/en/nuernberg/information/) cooperate with the goal of implementing an AI-supported exhibition. Together with researchers and international artists, new AI-based works of art will be realized during the next four years (2020-2023).  They will be embedded in the AI-supported exhibition in both houses. The Project „The Intelligent Museum” is funded by the Digital Culture Programme of the [Kulturstiftung des Bundes](https://www.kulturstiftung-des-bundes.de/en) (German Federal Cultural Foundation) and funded by the [Beauftragte der Bundesregierung für Kultur und Medien](https://www.bundesregierung.de/breg-de/bundesregierung/staatsministerin-fuer-kultur-und-medien) (Federal Government Commissioner for Culture and the Media).

As part of the project, digital curating will be critically examined using various approaches of digital art. Experimenting with new digital aesthetics and forms of expression enables new museum experiences and thus new ways of museum communication and visitor participation. The museum is transformed to a place of experience and critical exchange.

![Logo](media/Logo_ZKM_DMN_KSB.png)
