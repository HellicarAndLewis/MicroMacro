# MicroMacro
A series of camera and audio reactive installations for a fashion brand.

## Summary

Built using openFrameworks 0.84 on OSX 10.9. Uses [ofxRemoteUI](https://github.com/armadillu/ofxRemoteUI) as a remote GUI. Uses a Blackmagic Ultra Studio Mini Recorder to capture video but will fall back to using ofVideoGrabber if no device is present.

## Local Dev
### Project
-   Download openframeworks 0.84 and extract it
-   cd into apps and clone this repo `git clone git@github.com:HellicarAndLewis/MicroMacro.git`
-   cd into the repo root `cd MicroMacro` and clone additional addon dependencies using the script provided `. ./cloneaddons.sh`

### Blackmagic
Install Blackmagic Desktop Video 10.3.7 (see below) and restart. After installation, go to System Preferences, click "Black Magic Design" and make sure "Use 1080p not 1080PsF" is checked (this option is only available when the capture card is plugged in). If you don't check this option, nothing will work.

Then go to the support page and download the [DeckLink 10.1.4 SDK](https://www.blackmagicdesign.com/uk/support/family/capture-and-playback). After unzipping the SDK open the app Mac/Samples/bin/CapturePreview and select the video format of your device and hit "Start". If you have the right mode selected you should see the video streaming.

### Settings
Use the ofxRemoteUI OSX client to change app modes and settings locally. Compile your own from the [ofxRemoteUI repo](https://github.com/armadillu/ofxRemoteUI/tree/master/OSX%20Client) or use the precompiled version in this [repo's release section](https://github.com/HellicarAndLewis/MicroMacro/releases/tag/UIClientOSX).

## Dependencies
-	[openframeworks 0.84](http://www.openframeworks.cc/download/)
-   An [ofxRemoteUI](https://github.com/armadillu/ofxRemoteUI) client
-   Blackmagic [Desktop Video 10.3.7](https://www.blackmagicdesign.com/uk/support/family/capture-and-playback)

## Install notes

...