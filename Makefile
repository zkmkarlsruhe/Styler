# Attempt to load a config.make file.
# If none is found, project defaults in config.project.make will be used.
ifneq ($(wildcard config.make),)
	include config.make
endif

# make sure the the OF_ROOT location is defined
ifndef OF_ROOT
	OF_ROOT=$(realpath ../../of/of_v0.11.2_osx_release)
endif

# call the project makefile!
include $(OF_ROOT)/libs/openFrameworksCompiled/project/makefileCommon/compile.project.mk

# ofxTensorFlow2
include $(OF_ROOT)/addons/ofxTensorFlow2/addon_targets.mk

################################################################################
##### macos

# additional build targets for macOS
ifeq ($(PLATFORM_LIB_SUBPATH),osx)

##### Makefile-mac-dist.mk

# mac release app name
mac.app.name = Styler

# openFrameworks uses "APPNAME Debug" and "APPNAME Release" scheme naming
mac.app.project.scheme = Styler Release

# dist version string, ie. APPNAME-VERSION for dist dir naming
mac.dist.version := $(shell /usr/libexec/PlistBuddy -c "Print :CFBundleShortVersionString" openFrameworks-Info.plist)

# include these in the release dist package (relative to this dir)
mac.dist.include = bin/data CHANGES.txt LICENSE.txt README.md

# remove these from the release dist package (relative to the dist dir)
mac.dist.exclude = data/icon.icns data/icon-debug.icns data/output

# add link to /Applications in dmg
mac.dmg.appslink = true

# codesign identity, usually a Developer ID Application string
# REPLACE this with your own or set it via the commandline:
# make app dist-dmg mac.codesign.identity="Foo Bar Baz Developers"
#mac.codesign.identity = Foo

MACDISTHELPER_DIR=.
include $(MACDISTHELPER_DIR)/Makefile-mac-dist.mk

# rename to txt
postdistdir::
	cd $(mac.dist.subdir) && if [ -f README.md ] ; then mv README.md README.txt ; fi

# override zip and dmg naming to include platform and arch
mac.dmg.name = $(mac.dist.name.version)-macos-$(shell uname -m)
mac.zip.name = $(mac.dist.name.version)-macos-$(shell uname -m)

endif # osx
