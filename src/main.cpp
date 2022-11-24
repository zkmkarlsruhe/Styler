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
#include "ofMain.h"
#include "ofApp.h"

#include "Commandline.h"

//========================================================================
int main(int argc, char **argv) {
    ofApp *app = new ofApp();

    // parse commandline
    Commandline *parser = new Commandline(app);
    if(!parser->parse(argc, argv)) {
        return parser->exit();
    }
    delete parser; // done

    // run app
    ofSetupOpenGL(app->size.width, app->size.height, OF_WINDOW);
    ofRunApp(app);

    return EXIT_SUCCESS;
}
