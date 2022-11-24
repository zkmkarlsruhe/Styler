#
# Dan Wilcox <danomatika@gmail.com> 2022
#

SRC=../icon.png

ICNS=icon.icns
ICONSET=icon.iconset

ICNS-DEBUG=icon-debug.icns

.PHONY: clean

all: $(ICNS) $(ICNS-DEBUG)

# macOS iconset
$(ICNS): $(SRC)
	mkdir $(ICONSET)
	sips -z 16 16     $(SRC) --out $(ICONSET)/icon_16x16.png
	sips -z 32 32     $(SRC) --out $(ICONSET)/icon_16x16@2x.png
	sips -z 32 32     $(SRC) --out $(ICONSET)/icon_32x32.png
	sips -z 64 64     $(SRC) --out $(ICONSET)/icon_32x32@2x.png
	sips -z 128 128   $(SRC) --out $(ICONSET)/icon_128x128.png
	sips -z 256 256   $(SRC) --out $(ICONSET)/icon_128x128@2x.png
	sips -z 256 256   $(SRC) --out $(ICONSET)/icon_256x256.png
	sips -z 512 512   $(SRC) --out $(ICONSET)/icon_256x256@2x.png
	sips -z 512 512   $(SRC) --out $(ICONSET)/icon_512x512.png
	cp                $(SRC)       $(ICONSET)/icon_512x512@2x.png
	iconutil -c icns $(ICONSET)
	rm -rf $(ICONSET)

# debug macOS iconset
$(ICNS-DEBUG): $(ICNS)
	cp $(ICNS) $(ICNS-DEBUG)

clean:
	rm -f $(ICNS) $(ICNS-DEBUG)
