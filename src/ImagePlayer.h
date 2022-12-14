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
 */
#pragma once

#include "ofBaseTypes.h"
#include "ofUtils.h"

// mimic ofVideoPlayer but just show still images
class ImagePlayer {

	public:

		bool load(const std::vector<std::string> & paths) {
			if(paths.empty()) {return false;}
			this->paths = paths;
			setImage(this->paths[index]);
			return true;
		}

		void close() {
			stop();
			image.clear();
		}

		void update() {
			if(playing && !paused) { // auto advance frame?
				if(ofGetElapsedTimeMillis() - timestamp >= frameTime) {
					nextFrame();
					timestamp = ofGetElapsedTimeMillis();
				}
			}
			if(frameSet) { // new frame
				newFrame = true;
				frameSet = false;
			}
			else { // nothing to do
				newFrame = false;
			}
		}

		void draw(float x, float y) {
			image.draw(x, y);
		}

		void draw(float x, float y, float w, float h) {
			image.draw(x, y, w, h);
		}

		bool isFrameNew() const {return newFrame;}

		void setPaused(bool paused) {
			if(playing && (this->paused && !paused)) {
				timestamp = ofGetElapsedTimeMillis();
			}
			this->paused = paused;
		}
		bool isPaused() const {return paused;}
		void play() {
			if(index != 0) {
				index = 0;
				setImage(this->paths[index]);
			}
			timestamp = ofGetElapsedTimeMillis();
			paused = false;
			playing = true;
		}
		void stop() {
			playing = false;
			paused = false;
		}

		bool isLoaded() const {return image.isAllocated();}
		bool isPlaying() const {return playing;}

		void previousFrame() {
			if(index == 0) {
				index = paths.size()-1;
			}
			else {
				index--;
			}
			setImage(paths[index]);
		}

		void nextFrame() {
			index++;
			if(index >= paths.size()) {
				index = 0;
			}
			setImage(paths[index]);
		}

		void setFrameTime(int ms) {frameTime = ms;}
		int getFrameTime() {return frameTime;}

		float getWidth() const {return image.getWidth();}
		float getHeight() const {return image.getHeight();}

		const ofPixels & getPixels() const {return image.getPixels();}

		bool isLastFrame() {return index == paths.size()-1;}

	protected:

		void setImage(std::string & path) {
			if(!image.load(path)) {
				return;
			}
			if(image.getImageType() != OF_IMAGE_COLOR) {
				// model requires RGB without alpha, this is expensive
				image.setImageType(OF_IMAGE_COLOR);
			}
			frameSet = true; // signal new frame in update()
		}

	    std::vector<std::string> paths; ///< image file paths, min 1 required
		std::size_t index = 0; ///< image path index
		ofImage image; ///< current image
		bool newFrame = false; ///< outward frame change indicator
		bool frameSet = false; ///< inward frame change indicator
		bool playing = false; ///< is playback enabled?
		bool paused = false; ///< is playback paused?
		long timestamp = 0; ///< playback timestamp
		int frameTime = 1000; ///< playback frame time in ms
};
