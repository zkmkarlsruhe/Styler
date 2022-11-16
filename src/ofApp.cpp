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
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofSetWindowTitle("Styler");
	ofBackground(0);

	if(!styleTransfer.setup(imageWidth, imageHeight)) {
		std::exit(EXIT_FAILURE);
	}
	setStyle(stylePaths[styleIndex]);

	// input source
	setCameraSource();
	source.image.player.setFrameTime(3000);

	// output image
	scaler.width = imageWidth;
	scaler.height = imageHeight;
	ofSetWindowShape(imageWidth, imageHeight);

	styleTransfer.startThread();
}

//--------------------------------------------------------------
void ofApp::update() {
	source.current->update();
	if(source.current->isFrameNew() || updateFrame) {
		if(source.current == &source.camera && (mirror.vert || mirror.horz)) {
			ofPixels pixels(source.current->getPixels());
			pixels.mirror(mirror.vert, mirror.horz);
			styleTransfer.setInput(pixels);
		}
		else {
			styleTransfer.setInput(source.current->getPixels());
		}
		updateFrame = false;
	}
	styleTransfer.update();
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofPushMatrix();
		scaler.apply();
		styleTransfer.draw(0, 0);
	ofPopMatrix();

	if(debug) {
		// help
		std::string text;
		if(source.current == &source.image) {
			text = "source: images\n";
		}
		else if(source.current == &source.video) {
			text = "source: video\n";
		}
		else if(source.current == &source.camera) {
			text = "source: camera\n";
		}
		text += "v: video input\n"
		        "c: camera input\n"
		        "i: image input\n"
	            "m: mirror camera\n"
		        "n: flip camera\n"
		        "r: restart video\n"
		        "f: toggle fullscreen\n"
		        "s: save image\n"
		        "right: next style\n"
		        "left: prev style\n"
		        "space: toggle playback\n"
		        "up: next frame\n"
		        "down: prev frame\n"
				"drag&drop style image";
		ofDrawBitmapStringHighlight(text, 4, 12);

		// fps
		//ofDrawBitmapStringHighlight(ofToString((int)ofGetFrameRate()), ofGetWidth()-20, 12);
	}
}

//--------------------------------------------------------------
void ofApp::exit() {
	styleTransfer.stopThread();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch(key) {
		case OF_KEY_LEFT:
			prevStyle();
			if(source.current->isPaused()) {
				updateFrame = true;
			}
			break;
		case OF_KEY_RIGHT:
			nextStyle();
			if(source.current->isPaused()) {
				updateFrame = true;
			}
			break;
		case OF_KEY_UP:
			if(source.current->isPaused()) {
				source.current->nextFrame();
			}
			break;
		case OF_KEY_DOWN:
			if(source.current->isPaused()) {
				source.current->previousFrame();
			}
			break;
		case 'v': setVideoSource(); break;
		case 'c': setCameraSource(); break;
		case 'i': setImageSource(); break;
		case 'm':
			mirror.horz = !mirror.horz;
			break;
		case 'n':
			mirror.vert = !mirror.vert;
			break;
		case ' ':
			source.current->setPaused(!source.current->isPaused());
			break;
		case 'r':
			// restart video
			source.current->stop();
			source.current->play();
			break;
		case 'f':
			ofToggleFullscreen();
			break;
		case 's':
			ofDirectory::createDirectory("output");
			ofSaveImage(styleTransfer.getOutput().getPixels(),
			            "output/"+ofGetTimestampString("%m-%d-%Y_%h-%M-%S")+".png");
			break;
		case 'd':
			debug = !debug;
			break;
		default: break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	scaler.update(w, h);
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
	if(dragInfo.files.size() == 0) {return;}
	setStyle(dragInfo.files[0]);
	if(source.current->isPaused()) {
		updateFrame = true;
	}
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::prevStyle() {
	if(styleIndex == 0) {
		styleIndex = stylePaths.size()-1;
	}
	else {
		styleIndex--;
	}
	setStyle(stylePaths[styleIndex]);
}

//--------------------------------------------------------------
void ofApp::nextStyle() {
	styleIndex++;
	if(styleIndex >= stylePaths.size()) {
		styleIndex = 0;
	}
	setStyle(stylePaths[styleIndex]);
}

//--------------------------------------------------------------
void ofApp::setStyle(std::string & path) {
	ofImage style;
	if(!style.load(path)) {
		return;
	}
	ofLog() << "style: " << ofFilePath::getFileName(path);
	styleTransfer.setStyle(style.getPixels());
}

//--------------------------------------------------------------
// TODO: find movie by path?
void ofApp::setVideoSource() {
	source.camera.close();
	source.image.close();
	source.video.open(videoPath);
	source.video.player.setVolume(0);
	source.video.play();
	source.current = &source.video;
}

//--------------------------------------------------------------
void ofApp::setCameraSource() {
	source.video.close();
	source.image.close();
	source.camera.setup(imageWidth, imageHeight);
	source.current = &source.camera;
}

//--------------------------------------------------------------
void ofApp::setImageSource() {
	source.video.close();
	source.camera.close();
	source.image.open(imagePaths);
	//source.image.play();
	source.current = &source.image;
}
