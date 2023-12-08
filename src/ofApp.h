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
#include "ofxOsc.h"
#include "Source.h"
#include "Scaler.h"
#include "config.h"

/// advanced arbitrary style transfer which can dynamically change between input
/// sources: static images, video files, camera
class ofApp : public ofBaseApp {

	public:
		ofApp();

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

		/// osc receiver callback
		void oscReceived(const ofxOscMessage &message);

		/// goto prev style in the stylePaths vector
		void prevStyle();

		/// goto next style in the stylePaths vector
		void nextStyle();

		/// set style from given input image
		void setStyle(std::string & path);

		/// take current source frame as style image
		/// optionally saves style image if styleSave = true
		void takeStyle();

		/// switch to video source
		void setVideoSource();

		/// switch to live camera source
		void setCameraSource();

		/// switch to image source
		void setImageSource();

		/// update scaler for model output image
		void updateScalerModel();

		/// update scaler for current source
		void updateScalerSource();

		/// update style input & camera draw rect from win size
		void updateStyleInputRects();

		/// save the current style image
		void saveStyleImage();

		/// save current output image
		void saveOutputImage();

		// config settings
		CameraSourceSettings cameraSettings;
		CameraSourceSettings styleCameraSettings;

		/// helper to get jpg & png paths in a given directory
		std::vector<std::string> listImagePaths(std::string dirPath);

		/// helper to get mov, mp4, & avi paths in a given directory
		std::vector<std::string> listVideoPaths(std::string dirPath);

		ofxStyleTransfer styleTransfer; ///< model
		Scaler scaler; ///< scale output to window, keeps aspect

		bool debug = false; ///< show debug info?
		bool updateFrame = false; ///< update current output?
		bool stylePip = false; ///< draw style input & camera pip?
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
		std::vector<std::string> imagePaths;
		std::vector<std::string> videoPaths;

		// style source
		struct {
			Source *current = nullptr; ///< current style input
			CameraSource *camera = nullptr; ///< optional second camera input
		} styleSource;
		ofImage styleImage; ///< current style input image
		ofRectangle styleImageRect; ///< style image draw rect
		ofRectangle styleCameraRect; ///< style camera draw rect
		bool styleSave = false; ///< save style images when saving?

		// input / output sizes
		struct {
			int width = 1;
			int height = 1;
		} size; ///< current input & output size
		bool staticSize = true; ///< keep fixed size, do not change based on input?
		bool startFullscreen = false; ///< start in fullscreen?

		std::vector<std::string> stylePaths; ///< paths to available style images
		std::size_t styleIndex = 0; ///< current style path index

		// osc
		struct {
			int port = -1; ///< receiver port
			ofxOscReceiver *receiver = nullptr; ///< receiver, if used
		} osc;
};
