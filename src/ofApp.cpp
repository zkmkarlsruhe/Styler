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

// style input pip border
#define PIP_BORDER        5
#define PIP_BORDER2       10
#define PIP_BORDER_COLOR  180
#define PIP_BORDER_RADIUS 5

// style save indicator
#define SAVE_COLOR  220,0,0
#define SAVE_RADIUS 5

//--------------------------------------------------------------
ofApp::ofApp() : scaler(1, 1) {}

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofSetWindowTitle("Styler");
	ofBackground(0);

	// find style image paths
	stylePaths = listImagePaths("style");
	if(stylePaths.empty()) {
		ofLogError(PACKAGE) << "no style images found in bin/data/style";
		ofSystemAlertDialog("No style images found in bin/data/style");
		std::exit(EXIT_FAILURE);
	}

	// find input image paths
	imagePaths = listImagePaths("image");
	if(imagePaths.empty()) {
		ofLogWarning(PACKAGE) << "no images found in bin/data/image";
	}

	// find input video paths
	videoPaths = listVideoPaths("video");
	if(videoPaths.empty()) {
		ofLogWarning(PACKAGE) << "no videos found in bin/data/video";
	}

	// input source
	setCameraSource();
	source.image.player.setFrameTime(3000);
	size.width = source.current->getWidth();
	size.height = source.current->getHeight();

	// style camera source
	if(styleCameraSettings.device >= 0) {
		styleSource.camera = new CameraSource;
		if(styleSource.camera->setup(styleCameraSettings)) {
			styleSource.current = styleSource.camera;
			ofLogVerbose(PACKAGE) << "style camera source: "
				<< styleCameraSettings.device;
		}
		else {
			delete styleSource.camera;
			styleSource.camera = nullptr;
			ofLogError(PACKAGE) << "could not open style camera source";
		}
	}

	// output image
	scaler.setSize(size.width, size.height);
	ofSetWindowShape(size.width, size.height);

	// load model
	if(!styleTransfer.setup(size.width, size.height)) {
		std::exit(EXIT_FAILURE);
	}
	setStyle(stylePaths[styleIndex]);
	styleTransfer.startThread();

	// start receiver, if any
	if(osc.port > 0) {
		osc.receiver = new ofxOscReceiver();
		if(!osc.receiver->setup(osc.port)) {
			delete osc.receiver;
			osc.receiver = nullptr;
		}
	}

	// summary
	ofLogVerbose(PACKAGE) << "size: " << size.width << "x" << size.height;
	ofLogVerbose(PACKAGE) << "static size: " << (staticSize ? "true" : "false");
	ofLogVerbose(PACKAGE) << "style auto: " << (styleAuto ? "true" : "false");
	ofLogVerbose(PACKAGE) << "style auto time (camera): " << styleAutoTime;
	ofLogVerbose(PACKAGE) << "style save: " << (styleSave ? "true" : "false");
	ofLogVerbose(PACKAGE) << stylePaths.size() << " styles:";
	for(auto p : stylePaths) {ofLogVerbose(PACKAGE) << "" << p;}
	ofLogVerbose(PACKAGE) << imagePaths.size() << " images:";
	for(auto p : imagePaths) {ofLogVerbose(PACKAGE) << "" << p;}
	ofLogVerbose(PACKAGE) << videoPaths.size() << " videos:";
	for(auto p : videoPaths) {ofLogVerbose(PACKAGE) << "" << p;}
	if(osc.receiver) {
		ofLogVerbose(PACKAGE) << "receiver port: " << osc.port;
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	// process received osc events
	if(osc.receiver) {
		while(osc.receiver->hasWaitingMessages()) {
			ofxOscMessage message;
			if(osc.receiver->getNextMessage(message)) {
				oscReceived(message);
			}
		}
	}

	// update source frame?
	source.current->update();
	if(source.current->isFrameNew() || updateFrame) {

		// update to new source size?
		if(!staticSize &&
		   (size.width != source.current->getWidth() ||
		    size.height != source.current->getHeight())) {
			size.width = source.current->getWidth();
			size.height = source.current->getHeight();
			styleTransfer.setSize(size.width, size.height);
			if(styleSource.current && !styleSource.camera) {
				updateScalerSource();
			}
			ofLogVerbose(PACKAGE) << "size now " << size.width << " " << size.height;
		}

		// auto style transfer?
		if(styleAuto && !updateFrame) {
			if(source.current == &source.camera) {
				if(ofGetElapsedTimef() - styleAutoTimestamp > styleAutoTime) {
					nextStyle(); // camera: change on timer
					styleAutoTimestamp = ofGetElapsedTimef();
				}
			}
			else if(wasLastFrame && !source.current->isPaused()) {
				nextStyle(); // image(s) & video: change after last frame
			}
		}

		// input frame
		styleTransfer.setInput(source.current->getPixels());
		updateFrame = false;
		wasLastFrame = source.current->isLastFrame();
	}
	if(styleTransfer.update()) {
		if(scaler.width != styleTransfer.getOutput().getWidth() ||
		   scaler.height != styleTransfer.getOutput().getHeight()) {
			updateScalerModel(); // output size changed
		}
	}
	if(styleSource.camera) {
		styleSource.camera->update();
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	// start later for X on Linux
	if(startFullscreen) {
		ofToggleFullscreen();
		startFullscreen = false;
	}

	ofSetColor(255);

	ofPushMatrix();
		scaler.apply();
		if(styleSource.current && styleSource.current != styleSource.camera) {
			styleSource.current->draw(0, 0, scaler.width, scaler.height);
		}
		else {
			styleTransfer.draw(0, 0);
		}
	ofPopMatrix();

	// style input images
	if(stylePip) {
		if(styleSource.camera) {
			ofSetColor(PIP_BORDER_COLOR);
			ofDrawRectRounded(styleCameraRect.x - PIP_BORDER, styleCameraRect.y - PIP_BORDER,
				styleCameraRect.width + PIP_BORDER2, styleCameraRect.height + PIP_BORDER2, PIP_BORDER_RADIUS);
			ofSetColor(255);
			styleSource.camera->draw(styleCameraRect.x, styleCameraRect.y,
				styleCameraRect.width, styleCameraRect.height);
		}
		ofSetColor(PIP_BORDER_COLOR);
		ofDrawRectRounded(styleImageRect.x - PIP_BORDER, styleImageRect.y - PIP_BORDER,
			styleImageRect.width + PIP_BORDER2, styleImageRect.height + PIP_BORDER2, PIP_BORDER_RADIUS);
		ofSetColor(255);
		styleImage.draw(styleImageRect.x, styleImageRect.y,
			styleImageRect.width, styleImageRect.height);
	}

	// style save status
	if(styleSave) {
		ofSetColor(SAVE_COLOR);
		ofDrawCircle(ofGetWidth() - SAVE_RADIUS - PIP_BORDER - 1, SAVE_RADIUS + PIP_BORDER + 1, SAVE_RADIUS);
	}

	// help
	if(debug) {
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
		        "m: mirror camera";
		if(styleSource.camera) {
		text += " / (shift) style camera";
		}
		text += "\n"
		        "n: flip camera";
		if(styleSource.camera) {
		text += " / (shift) style camera";
		}
		text += "\n"
		        "r: restart video\n"
		        "f: toggle fullscreen\n"
		        "s: save image / (shift) toggle style save\n";
		if(!styleSource.camera) {
		text += "k: toggle style input mode\n";
		}
		text += "p: toggle style input pip\n"
		        "a: toggle auto style change\n"
		        "right: next style\n"
		        "left: prev style\n"
		        "space: toggle playback / take style image\n"
		        "up: next frame / (shift) next video\n"
		        "down: prev frame / (shift) prev video\n"
		        "shift+m: \n"
		        "shift+n: \n",
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
			if(ofGetKeyPressed(OF_KEY_SHIFT)) {
				if(source.current == &source.video) {
					source.video.player.nextVideo();
				}
			}
			else {
				if(source.current->isPaused()) {
					source.current->nextFrame();
				}
			}
			break;
		case OF_KEY_DOWN:
			if(ofGetKeyPressed(OF_KEY_SHIFT)) {
				if(source.current == &source.video) {
					source.video.player.previousVideo();
				}
			}
			else {
				if(source.current->isPaused()) {
					source.current->previousFrame();
				}
			}
			break;
		case 'v': setVideoSource(); break;
		case 'c': setCameraSource(); break;
		case 'i': setImageSource(); break;
		case 'm':
			source.camera.mirror.horz = !source.camera.mirror.horz;
			break;
		case 'M':
			if(styleSource.camera) {
				styleSource.camera->mirror.horz = !styleSource.camera->mirror.horz;
			}
			break;
		case 'n':
			source.camera.mirror.vert = !source.camera.mirror.vert;
			break;
		case 'N':
			if(styleSource.camera) {
				styleSource.camera->mirror.vert = !styleSource.camera->mirror.vert;
			}
			break;
		case ' ':
			if(!styleSource.current) {
				source.current->setPaused(!source.current->isPaused());
			}
			else {
				takeStyle();
			}
			break;
		case 'r':
			// restart video
			source.current->stop();
			source.current->play();
			break;
		case 'k':
			if(!styleSource.camera) {
				if(styleSource.current) {
					styleSource.current = nullptr;
					updateScalerModel();
				}
				else {
					styleSource.current = source.current;
					updateScalerSource();
				}
			}
			break;
		case 'p':
			stylePip = !stylePip;
			break;
		case 'a':
			styleAuto = !styleAuto;
			if(styleAuto) {
				styleAutoTimestamp = ofGetElapsedTimef();
			}
			ofLogVerbose(PACKAGE) << "style auto: " << (int)styleAuto;
			break;
		case 'f':
			ofToggleFullscreen();
			break;
		case 's':
			saveOutputImage();
			break;
		case 'S':
			styleSave = !styleSave;
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
	updateStyleInputRects();
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
void ofApp::oscReceived(const ofxOscMessage &message) {
	if(message.getAddress() == "/style/take") {
		takeStyle();
	}
	else if(message.getAddress() == "/style/save") {
		saveStyleImage();
	}
	else if(message.getAddress() == "/output/save") {
		if(message.getNumArgs() == 0) {
                        saveOutputImage();
                }
                else if((message.getTypeString() == "i" || message.getTypeString() == "f")
                        && message.getArgAsInt(0) > 0) {
                        saveOutputImage();
                }

	}
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
	if(style.getImageType() != OF_IMAGE_COLOR) {
		// model requires RGB without alpha, this is expensive
		style.setImageType(OF_IMAGE_COLOR);
	}
	ofLogVerbose(PACKAGE) << "style now " << ofFilePath::getFileName(path);
	styleTransfer.setStyle(style.getPixels());
	styleImage.setFromPixels(style.getPixels());
	updateStyleInputRects();
}

//--------------------------------------------------------------
void ofApp::takeStyle() {
	if(styleSource.current) {
		styleTransfer.setStyle(styleSource.current->getPixels());
		styleImage.setFromPixels(styleSource.current->getPixels());
		updateStyleInputRects();
	}
	if(!styleSource.camera) { // done
		styleSource.current = nullptr;
		updateScalerModel();
	}
	if(styleSave) {
		saveStyleImage();
	}
}

//--------------------------------------------------------------
void ofApp::saveStyleImage() {
	ofDirectory::createDirectory("output-style");
	std::string path = "output-style/"+ofGetTimestampString("%m-%d-%Y_%H-%M-%S")+".png";
	ofSaveImage(styleImage.getPixels(), path);
	ofLogVerbose(PACKAGE) << "saved style " << path;
}

//--------------------------------------------------------------
void ofApp::saveOutputImage() {
	ofDirectory::createDirectory("output");
	std::string path = "output/"+ofGetTimestampString("%m-%d-%Y_%H-%M-%S")+".png";
	ofSaveImage(styleTransfer.getOutput().getPixels(), path);
	ofLogVerbose(PACKAGE) << "saved " << path;
}

//--------------------------------------------------------------
void ofApp::setVideoSource() {
	if(!source.video.open(videoPaths)) {return;}
	source.video.play();
	source.video.setVolume(0);
	source.current = &source.video;
	source.camera.close();
	source.image.close();
	wasLastFrame = false;
	styleAutoTimestamp = ofGetElapsedTimef();
	ofLogVerbose(PACKAGE) << "video source";
}

//--------------------------------------------------------------
void ofApp::setCameraSource() {
	source.camera.setup(cameraSettings);
	source.current = &source.camera;
	source.video.close();
	source.image.close();
	wasLastFrame = false;
	styleAutoTimestamp = ofGetElapsedTimef();
	ofLogVerbose(PACKAGE) << "camera source";
}

//--------------------------------------------------------------
void ofApp::setImageSource() {
	if(!source.image.open(imagePaths)) {return;}
	source.image.play();
	source.current = &source.image;
	source.video.close();
	source.camera.close();
	wasLastFrame = false;
	styleAutoTimestamp = ofGetElapsedTimef();
	ofLogVerbose(PACKAGE) << "image source";
}

//--------------------------------------------------------------
void ofApp::updateScalerModel() {
	scaler.setSize(styleTransfer.getOutput().getWidth(),
	               styleTransfer.getOutput().getHeight());
	scaler.update();
}

//--------------------------------------------------------------
void ofApp::updateScalerSource() {
	if(!staticSize) {
		scaler.setSize(size.width, size.height);
	}
	scaler.update();
}

//--------------------------------------------------------------
void ofApp::updateStyleInputRects() {
	float w = (float)ofGetWidth() / 5.f;
	float h = 0, x = ofGetWidth() - w - PIP_BORDER, y = PIP_BORDER;
	if(styleSource.camera) {
		h = (float)styleSource.camera->getHeight() * (w / (float)styleSource.camera->getWidth());
		styleCameraRect.set(x, y, w, h);
		y = h + PIP_BORDER2;
	}
	h = (float)styleImage.getHeight() * (w / (float)styleImage.getWidth());
	styleImageRect.set(x, y, w, h);
}

//--------------------------------------------------------------
std::vector<std::string> ofApp::listImagePaths(std::string dirPath) {
	ofDirectory dir(dirPath);
	dir.allowExt("jpg");
	dir.allowExt("JPG");
	dir.allowExt("jpeg");
	dir.allowExt("png");
	dir.allowExt("PNG");
	dir.sort();
	std::size_t count = dir.listDir();
	std::vector<std::string> paths;
	for(std::size_t i = 0; i < count; ++i) {
		paths.push_back(dir.getPath(i));
	}
	return paths;
}

//--------------------------------------------------------------
// TODO: add more extensions?
std::vector<std::string> ofApp::listVideoPaths(std::string dirPath) {
	ofDirectory dir(dirPath);
	dir.allowExt("mov");
	dir.allowExt("MOV");
	dir.allowExt("mp4");
	dir.allowExt("MP4");
	dir.allowExt("avi");
	dir.allowExt("AVI");
	dir.sort();
	std::size_t count = dir.listDir();
	std::vector<std::string> paths;
	for(std::size_t i = 0; i < count; ++i) {
		paths.push_back(dir.getPath(i));
	}
	return paths;
}
