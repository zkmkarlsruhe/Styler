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

#include "ImagePlayer.h"
#include "VideoPlayer.h"

/// base input frame source class
class Source {
	public:
		virtual ~Source() {}
		virtual void update() = 0;
		virtual void draw(float x, float y) = 0;
		virtual void draw(float x, float y, float w, float h) = 0;
		virtual void draw(ofRectangle & rect) {
			draw(rect.x, rect.y, rect.width, rect.height);
		}
		virtual bool isFrameNew() = 0;
		virtual const ofPixels & getPixels() = 0;
		virtual int getWidth()  = 0;
		virtual int getHeight() = 0;
		virtual void setPaused(bool paused) {}
		virtual bool isPaused() {return false;}
		virtual void nextFrame() {}
		virtual void previousFrame() {}
		virtual bool isLastFrame() {return false;}
		virtual void play() {}
		virtual void stop() {}
};

/// static image input source
class ImageSource : public Source {
	public:
		ImagePlayer player;
		bool open(const std::vector<std::string> & paths) {
			return player.load(paths);
		}
		void close() {player.close();}
		void update() {player.update();}
		void draw(float x, float y) {player.draw(x, y);}
		void draw(float x, float y, float w, float h) {player.draw(x, y, w, h);}
		bool isFrameNew() {return player.isFrameNew();}
		const ofPixels & getPixels() {return player.getPixels();}
		int getWidth() {return player.getWidth();}
		int getHeight() {return player.getHeight();}
		void setPaused(bool paused) {player.setPaused(paused);}
		bool isPaused() {return player.isPaused();}
		void nextFrame() {player.nextFrame();}
		void previousFrame() {player.previousFrame();}
		bool isLastFrame() {return player.isLastFrame();}
		void play() {player.play();}
		void stop() {player.stop();}
};

/// video player source
class PlayerSource : public Source {
	public:
		VideoPlayer player;
		bool open(const std::vector<std::string> & paths) {
			return player.load(paths);
		}
		void close() {player.close();}
		void update() {player.update();}
		void draw(float x, float y) {player.draw(x, y);}
		void draw(float x, float y, float w, float h) {player.draw(x, y, w, h);}
		bool isFrameNew() {return player.isFrameNew();}
		const ofPixels & getPixels() {return player.getPixels();}
		int getWidth() {return player.getWidth();}
		int getHeight() {return player.getHeight();}
		void setPaused(bool paused) {player.setPaused(paused);}
		bool isPaused() {return player.isPaused();}
		void nextFrame() {player.nextFrame();}
		void previousFrame() {player.previousFrame();}
		bool isLastFrame() {return player.isLastFrame();}
		void play() {player.play();}
		void stop() {player.stop();}
		void setVolume(float v) {player.setVolume(v);}
};

/// camera input source settings
struct CameraSourceSettings {
	int device = 0; ///< desired input device id
	int rate = 30; ///< desired input framerate
	struct {
		int width = 640;
		int height = 480;
	} size; ///< desired input size
	struct {
		bool vert = false; ///< flip
		bool horz = false; ///< mirror
	} mirror;
};

/// camera input source
class CameraSource : public Source {
	public:
		// ofVideograbber doesn't seem to like setup()/close() cycles,
		// so dynamically create an instance each time
		ofVideoGrabber *grabber = nullptr;
		struct {
			bool vert = false; ///< flip
			bool horz = false; ///< mirror
		} mirror;
		ofImage image; ///< mirrored pixels buffer
		bool setup(const CameraSourceSettings & settings) {
			mirror.vert = settings.mirror.vert;
			mirror.horz = settings.mirror.horz;
			return setup(settings.size.width, settings.size.height,
			             settings.rate, settings.device);
		}
		bool setup(int w, int h, int fps=30, int deviceID=0) {
			close();
			grabber = new ofVideoGrabber();
			grabber->setDesiredFrameRate(fps);
			grabber->setDeviceID(deviceID);
			return grabber->setup(w, h);
		}
		void close() {
			if(grabber) {
				grabber->close();
				delete grabber;
				grabber = nullptr;
			}
		}
		void update() {
			grabber->update();
			if(grabber->isFrameNew()) {
				if(mirror.vert || mirror.horz) {
					image.setFromPixels(grabber->getPixels());
					image.mirror(mirror.vert, mirror.horz);
				}
				else if(image.isAllocated()) {
					image.clear();
				}
			}
		}
		void draw(float x, float y) {
			if(image.isAllocated()) {image.draw(x, y);}
			else {grabber->draw(x, y);}
		}
		void draw(float x, float y, float w, float h) {
			if(image.isAllocated()) {image.draw(x, y, w, h);}
			else {grabber->draw(x, y, w, h);}
		}
		bool isFrameNew() {return grabber->isFrameNew();}
		const ofPixels & getPixels() {
			if(image.isAllocated()) {return image.getPixels();}
			return grabber->getPixels();
		}
		int getWidth() {return grabber->getWidth();}
		int getHeight() {return grabber->getHeight();}
};
