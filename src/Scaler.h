// Copyright (c) 2018 Dan Wilcox <danomatika@gmail.com>
// GPL v3 License
// danomatika.com
#pragma once

#include "ofGraphics.h"
#include "ofRectangle.h"

/// scale from 2D render size to window size
/// ex:
///     Scale scaler(640, 480);
///     ...
///     ofApp::draw() {
///         ofPushMatrix();
///             scaler.apply();
///             int w = scaler.width, h = scaler.height;
///             ...
///         ofPopMatrix();
///     }
///     ...
///     ofApp::windowResized(int w, int h) {
///         scaler.update(w, h)
///     }
class Scaler {
	public:

		/// constructor with current window size
		Scaler() {
			width = ofGetWidth();
			height = ofGetHeight();
		}

		/// constructor with render pixel size
		Scaler(int w, int h) {
			width = w;
			height = h;
		}

		/// set render pixel size, call update after to put into effect
		void setSize(int w, int h) {
			width = w;
			height = h;
		}

		/// update scaler to fit window
		void update() {
			update(ofGetWidth(), ofGetHeight());
		}

		/// update scaler to fit given size
		void update(int w, int h) {
			ofRectangle win(0, 0, w, h);
			ofAspectRatioMode a = aspect ? OF_ASPECT_RATIO_KEEP : OF_ASPECT_RATIO_IGNORE;
			ofRectangle render(0, 0, width, height);
			render.scaleTo(win, a, OF_ALIGN_HORZ_CENTER, OF_ALIGN_VERT_CENTER);
			x = render.x;
			y = render.y;
			scaleX = render.width / width;
			scaleY = render.height / height;
		}

		/// apply render scaling, put this inside ofPushMatrix() & ofPopMatrix()
		void apply() {
			ofTranslate(x, y);
			ofScale(scaleX, scaleY);
			if(mirrorHorz) {
				ofRotateYDeg(180);
				ofTranslate(-width, 0, 0);
			}
			if(mirrorVert) {
				ofRotateXDeg(180);
				ofTranslate(0, -height, 0);
			}
		}

		/// convert screen mouseX position to render scale coordinates
		int mouseX(int mx=ofGetMouseX()) {
			return (mx - x) / scaleX;
		}

		/// convert screen mouseY position to render scale coordinates
		int mouseY(int my=ofGetMouseY()) {
			return (my - y) / scaleY;
		}

		int width = 1;           ///< render width in pixels
		int height = 1;          ///< render height in pixels
		float x = 0;             ///< upper left corner coordinate
		float y = 0;             ///< upper left cormer coordinate
		float scaleX = 1.0;      ///< horz scale between window & render width
		float scaleY = 1.0;      ///< vert scale between window & render height
		bool aspect = true;      ///< keep aspect ratio when scaling?
		bool mirrorHorz = false; ///< mirror horz?
		bool mirrorVert = false; ///< mirror vert?
};
