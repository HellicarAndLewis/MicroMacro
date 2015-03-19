# MicroMacro
A series of camera and audio reactive installations for a fashion brand.


## Summary
Built using openFrameworks 0.84 on OSX 10.10. Slitscan uses a Blackmagic Ultra Studio Mini Recorder to capture video but will fall back to using ofVideoGrabber if no device is present. Slitscan also uses [ofxRemoteUI](https://github.com/armadillu/ofxRemoteUI) as a remote GUI and can be launched with various arguments (see Settings & Control below). Controller is an app for controlling individual or groups of clients over OSC.


## Local Dev
### Project
-   Download openframeworks 0.84 and extract it
-   cd into apps and clone this repo `git clone git@github.com:HellicarAndLewis/MicroMacro.git`
-   cd into the repo root `cd MicroMacro` and clone additional addon dependencies using the script provided `. ./cloneaddons.sh`

### Blackmagic
Install Blackmagic Desktop Video 10.3.7 (see below) and restart. After installation, go to System Preferences, click "Black Magic Design" and make sure "Use 1080p not 1080PsF" is checked (this option is only available when the capture card is plugged in). If you don't check this option, nothing will work.

Then go to the support page and download the [DeckLink 10.1.4 SDK](https://www.blackmagicdesign.com/uk/support/family/capture-and-playback). After unzipping the SDK open the app Mac/Samples/bin/CapturePreview and select the video format of your device and hit "Start". If you have the right mode selected you should see the video streaming.

### Settings & Control
Use the ofxRemoteUI OSX client to change app modes and settings locally for a single app. Compile your own from the [ofxRemoteUI repo](https://github.com/armadillu/ofxRemoteUI/tree/master/OSX%20Client) or use the precompiled version in this [repo's release section](https://github.com/HellicarAndLewis/MicroMacro/releases/tag/UIClientOSX).

Use the controller app to control multiple client apps. Clients can be started with arguments to specify unique IDs from terminal or using the scripts in /scripts. Spawn multiple clients locally with different IDs (1-9) and control them via controller. The controller uses a broadcast address which defaults to 192.168.1.255 but can be set using app arguments e.g. `{path to app} --args -address="192.168.11.255"`

Slit scan can also be forced to capture from Blackmagic at 720p instead of the default 1080p by passing in the argument `-capture720=true` or by launching '720capture.command' in /scripts.


## Dependencies
-	[openframeworks 0.84](http://www.openframeworks.cc/download/)
-   An [ofxRemoteUI](https://github.com/armadillu/ofxRemoteUI) client
-   Blackmagic [Desktop Video 10.3.7](https://www.blackmagicdesign.com/uk/support/family/capture-and-playback)
-   The control application makes use of the LANs broadcast address and will not work if this feature is disabled or blocked on the local network.


## Install notes
All clients and the controller must be connected to the same network on the same IP range. The control application makes use of the LANs broadcast address and will not work if this feature is disabled or blocked on the local network.

Each client application should be configured to start up with a unique numerical ID using the provided scripts in the /scripts directory.
