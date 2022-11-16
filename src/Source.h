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

/// base input frame source class
class Source {
	public:
		virtual ~Source() {}
		virtual void update() = 0;
		virtual bool isFrameNew() = 0;
		virtual const ofPixels & getPixels() = 0;
		virtual int getWidth()  = 0;
		virtual int getHeight() = 0;
		virtual bool setPaused(bool paused) {}
		virtual bool isPaused() {return false;}
		virtual void nextFrame() {}
		virtual void previousFrame() {}
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
		bool isFrameNew() {return player.isFrameNew();}
		const ofPixels & getPixels() {return player.getPixels();}
		int getWidth() {return player.getWidth();}
		int getHeight() {return player.getHeight();}
		bool setPaused(bool paused) {player.setPaused(paused);}
		bool isPaused() {return player.isPaused();}
		void nextFrame() {player.nextFrame();}
		void previousFrame() {player.previousFrame();}
		void play() {player.play();}
		void stop() {player.stop();}
};

/// video player source
class PlayerSource : public Source {
	public:
		ofVideoPlayer player;
		bool open(const std::string & path) {
			return player.load(path);
		}
		void close() {player.close();}
		void update() {player.update();}
		bool isFrameNew() {return player.isFrameNew();}
		const ofPixels & getPixels() {return player.getPixels();}
		int getWidth() {return player.getWidth();}
		int getHeight() {return player.getHeight();}
		bool setPaused(bool paused) {player.setPaused(paused);}
		bool isPaused() {return player.isPaused();}
		void nextFrame() {player.nextFrame();}
		void previousFrame() {player.previousFrame();}
		void play() {player.play();}
		void stop() {player.stop();}
};

/// camera input source
class CameraSource : public Source {
	public:
		// ofVideograbber doesn't seem to like setup()/close() cycles,
		// so dynamically create an instance each time
		ofVideoGrabber *grabber = nullptr;
		bool setup(int w, int h, int deviceID=0) {
			close();
			grabber = new ofVideoGrabber();
			grabber->setDeviceID(deviceID);
			grabber->setDesiredFrameRate(30);
			return grabber->setup(w, h);
		}
		void close() {
			if(grabber) {
				grabber->close();
				delete grabber;
				grabber = nullptr;
			}
		}
		void update() {grabber->update();}
		bool isFrameNew() {return grabber->isFrameNew();}
		const ofPixels & getPixels() {return grabber->getPixels();}
		int getWidth() {return grabber->getWidth();}
		int getHeight() {return grabber->getHeight();}
		bool setPaused(bool paused) {}
		bool isPaused() {return false;}
		void nextFrame() {}
		void previousFrame() {}
};
