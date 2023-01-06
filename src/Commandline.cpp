/*
 * Styler
 *
 * Copyright (c) 2022 ZKM | Hertz-Lab
 * Dan Wilcox <dan.wilcox@zkm.de>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * This code has been developed at ZKM | Hertz-Lab as part of „The Intelligent
 * Museum“ generously funded by the German Federal Cultural Foundation.
 */
#include "Commandline.h"

static void setCameraSize(CameraSourceSettings &settings, std::string & size);

Commandline::Commandline(ofApp *app) : app(app) {
	parser.description(DESCRIPTION);
	parser.get_formatter()->label("Positionals", "Arguments");
}

bool Commandline::parse(int argc, char **argv) {

	// local options, the rest are ofAppSettings instance variables
	std::string size = "";
	std::string styleSize = "";
	bool list = false;
	bool styleMirror = false;
	bool styleFlip = false;
	bool verbose = false;
	bool version = false;
	std::string settings = "";

	// no style camera by default
	app->styleCameraSettings.device = -1;

	parser.add_flag("-f,--fullscreen", app->startFullscreen, "start in fullscreen");
	parser.add_flag("-a,--auto", app->styleAuto, "enable auto style change");
	parser.add_option("--auto-time", app->styleAutoTime, "set camera auto style change time in s, default " + ofToString(app->styleAutoTime));
	parser.add_option("-p,--port", app->osc.port, "OSC listen port, default none");
	parser.add_flag(  "-l,--list", list, "list camera devices and exit");
	parser.add_option("-d,--dev", app->cameraSettings.device, "camera device number, default " + ofToString(app->cameraSettings.device));
	parser.add_option("-r,--rate", app->cameraSettings.rate, "desired camera framerate, default " + ofToString(app->cameraSettings.rate));
	parser.add_option("-s,--size", size, "desired camera size, default " +
		ofToString(app->cameraSettings.size.width) + "x" + ofToString(app->cameraSettings.size.height));
	parser.add_flag("--mirror", app->cameraSettings.mirror.horz, "mirror camera horizontally");
	parser.add_flag("--flip", app->cameraSettings.mirror.vert, "flip camera vertically");
	parser.add_flag("--static-size", app->staticSize, "disable dynamic input -> output size handling");
	parser.add_option("--style-dev", app->styleCameraSettings.device, "optional second style camera device number");
	parser.add_option("--style-rate", app->cameraSettings.rate, "desired style camera framerate, default " + ofToString(app->styleCameraSettings.rate));
	parser.add_option("--style-size", styleSize, "desired style camera size, default " +
		ofToString(app->styleCameraSettings.size.width) + "x" + ofToString(app->styleCameraSettings.size.height));
	parser.add_flag("--style-mirror", app->styleCameraSettings.mirror.horz, "mirror style camera horizontally");
	parser.add_flag("--style-flip", app->styleCameraSettings.mirror.vert, "flip style camera vertically");
	parser.add_flag("-v,--verbose", verbose, "verbose printing");
	parser.add_flag("--version", version, "print version and exit");

	try {
		parser.parse(argc, argv);
	}
	catch(const CLI::ParseError &e) {
		error = e;
		return false;
	}

	// verbose printing?
	ofSetLogLevel(PACKAGE, (verbose ? OF_LOG_VERBOSE : OF_LOG_NOTICE));

	// print version
	if(version) {
		std::cout << VERSION << std::endl;
		return false;
	}

	// list audio input devices
	if(list) {
		ofVideoGrabber grabber;
		grabber.listDevices();
		return false;
	}

	// check osc port
	if(app->osc.port > 0) {
		if(app->osc.port <= 1024) {
			ofLogWarning(PACKAGE) << "ignoring invalid port number <= 1024: " << app->osc.port;
			app->osc.port = -1;
		}
	}

	// check style auto change time
	if(app->styleAutoTime <= 0) {
		ofLogWarning(PACKAGE) << "ignoring invalid auto style change time: " << app->styleAutoTime;
		app->styleAutoTime = 20;
	}

	// size: WxH, ie. 640x480 or 1280X720
	if(size != "") {
		setCameraSize(app->cameraSettings, size);
	}
	if(styleSize != "") {
		setCameraSize(app->styleCameraSettings, styleSize);
	}
	app->size.width = app->cameraSettings.size.width;
	app->size.height = app->cameraSettings.size.height;

	return true;
}

int Commandline::exit() {
	return parser.exit(error);
}

static void setCameraSize(CameraSourceSettings &settings, std::string & size) {
	std::size_t found = size.find_last_of("x");
	if(found == std::string::npos) {
		found = size.find_last_of("X"); // try uppercase too
	}
	if(found != std::string::npos) {
		int w = ofToInt(size.substr(0, found));
		int h = ofToInt(size.substr(found+1));
		if(w == 0 || h == 0) {
			ofLogWarning(PACKAGE) << "ignoring invalid size: " << size;
		}
		else {
			settings.size.width = w;
			settings.size.height = h;
		}
	}
	else {
		ofLogWarning(PACKAGE) << "ignoring invalid size: " << size;
	}
}
