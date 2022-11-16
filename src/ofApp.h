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

		void setVideoSource();
		void setCameraSource();
		void setImageSource();

		ofxStyleTransfer styleTransfer;
		ofImage outputImage;
		Scaler scaler; // scale output to window, keeps aspect

		bool debug = false;
		bool updateFrame = false;

		// input sources
		// TODO: build paths from image / video dir contents or config file
		struct {
			Source *current = nullptr; //< set this before using!
			PlayerSource video;
			CameraSource camera;
			ImageSource image;
		} source;
		struct {
			bool vert = false;
			bool horz = false;
		} mirror;
		std::vector<std::string> imagePaths = {
			"style/pand.png",
			"style/ani.png",
			"style/noob.png",
			"style/rab.png"
		};
		std::string videoPath = "movie.mp4";

		// image input & output size expected by model
		//const static int imageWidth = 1280;
		//const static int imageHeight = 720;
		const static int imageWidth = 1920;
		const static int imageHeight = 1080;
		//const static int imageWidth = 640;
		//const static int imageHeight = 360;
		//const static int imageWidth = 3840;
		//const static int imageHeight = 2160;

		// style image size expected by model
		static const int styleWidth = 256;
		static const int styleHeight = 256;

		bool styleInput = false;
		void takeStyle();

		// paths to available style images
	    // TODO: build paths from style dir contents or config file
		std::vector<std::string> stylePaths = {
			"style/pand.png",
			"style/ani.png",
			"style/noob.png",
			"style/rab.png"
		};
		std::size_t styleIndex = 0; // current model path index
};
