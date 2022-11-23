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

// wrap ofVideoPlayer with simple path playlist
class VideoPlayer {

	public:

		bool load(const std::vector<std::string> & paths) {
			if(paths.empty()) {return false;}
			this->paths = paths;
			setVideo(this->paths[index]);
			return true;
		}

		void close() {
			player.close();
		}

		void update() {
			player.update();
			if(player.getIsMovieDone()) {
				nextVideo();
			}
			else if(player.isFrameNew() && newVideo) {
				newVideo = false;
			}
		}

		void draw(float x, float y) {
			player.draw(x, y);
		}

		void draw(float x, float y, float w, float h) {
			player.draw(x, y, w, h);
		}

		bool isFrameNew() const {
			return player.isFrameNew();
		}

		void setPaused(bool paused) {
			player.setPaused(paused);
		}

		bool isPaused() const {
			return player.isPaused();
		}

		void play() {
			player.play();
		}
		void stop() {
			player.stop();
		}

		bool isLoaded() const {return player.isLoaded();}
		bool isPlaying() const {return player.isPlaying();}

		void previousFrame() {
			player.previousFrame();
		}

		void nextFrame() {
			player.nextFrame();
		}

		float getWidth() const {return player.getWidth();}
		float getHeight() const {return player.getHeight();}

		const ofPixels & getPixels() const {return player.getPixels();}

		bool isLastFrame() {return player.getCurrentFrame() == player.getTotalNumFrames();}

		bool isVideoNew() {return newVideo;}

		void previousVideo() {
			if(index == 0) {
				index = paths.size()-1;
			}
			else {
				index--;
			}
			setVideo(paths[index]);
		}

		void nextVideo() {
			index++;
			if(index >= paths.size()) {
				index = 0;
			}
			setVideo(paths[index]);
		}

		void setVolume(float v) {player.setVolume(v);}

	protected:

		void setVideo(std::string & path) {
			if(!player.load(path)) {
				return;
			}
			newVideo = true;
			ofLog() << "video: " << path;
		}

		std::vector<std::string> paths; ///< image file paths, min 1 required
		std::size_t index = 0; ///< image path index
		ofVideoPlayer player; ///< current video
		bool newVideo = false; ///< has a new video been loaded?
};
