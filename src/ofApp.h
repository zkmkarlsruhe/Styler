/*
 * Styler
 *
 * Copyright (c) 2022 ZKM | Hertz-Lab
 * Dan Wilcox <dan.wilcox@zkm.de>
 *
 * GPL v3 License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * This code has been developed at ZKM | Hertz-Lab as part of „The Intelligent
 * Museum“ generously funded by the German Federal Cultural Foundation.
 *
 * Adapted from ofxTensorFlow2 example_style_transfer_arbitrary
 */
#pragma once

#include "ofMain.h"
#include "ofxTensorFlow2.h"
#include "ofxStyleTransfer.h"
#include "Source.h"
#include "Scaler.h"

/// advanced aribtrary style transfer which can dynamically change between input
/// sources: static images, video file, camera
class ofApp : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		/// goto prev style in the stylePaths vector
		void prevStyle();

		/// goto next style in the stylePaths vector
		void nextStyle();

		/// set style from given input image
		void setStyle(std::string & path);

		/// take current source frame as style image
		void takeStyle();

		/// switch to video source
		void setVideoSource();

		/// switch to live camera source
		void setCameraSource();

		/// switch to image source
		void setImageSource();

		/// helper to get jpg & png paths in a given directory
		std::vector<std::string> listImagePaths(std::string dirPath);

		/// helper to get mov, mp4, & avi paths in a given directory
		std::vector<std::string> listVideoPaths(std::string dirPath);

		ofxStyleTransfer styleTransfer; ///< model
		Scaler scaler; ///< scale output to window, keeps aspect

		bool debug = false; ///< show debug info?
		bool updateFrame = false; ///< update current output?
		bool styleInput = false; ///< style input mode?
		bool styleAuto = false;  ///< change style automatically?
		bool wasLastFrame = false; ///< was the prev source frame the last?

		/// timestamp in s for last style change, only used for camera source
		float styleAutoTimestamp = 0;

		/// style change time in s, only used for camera source
		float styleAutoTime = 20;

		// input sources
		struct {
			Source *current = nullptr; ///< set this before using!
			PlayerSource video;
			CameraSource camera;
			ImageSource image;
		} source;
		struct {
			bool vert = false;
			bool horz = false;
		} mirror;
		std::vector<std::string> imagePaths;
		std::vector<std::string> videoPaths;

		// image input & output size
		//const static int imageWidth = 1280;
		//const static int imageHeight = 720;
		//const static int imageWidth = 1920;
		//const static int imageHeight = 1080;
		const static int imageWidth = 640;
		const static int imageHeight = 480;
		//const static int imageWidth = 3840;
		//const static int imageHeight = 2160;

		std::vector<std::string> stylePaths; ///< paths to available style images
		std::size_t styleIndex = 0; ///< current style path index
};
