#include "ofApp.h"


void ofApp::setup(){
    
    // defaults, overriden by any saved ofxRemoteUI settings
    isDebug = true;
    bgColour.set(0);
    appMode == SLIT_SCAN;
    
    // Using ofxRemoteUI https://github.com/armadillu/ofxRemoteUI/
    // optionaly specify port here, otherwise random
	RUI_SETUP(4000);
	ofAddListener(RUI_GET_OF_EVENT(), this, &ofApp::clientDidSomething);
	RUI_GET_INSTANCE()->setVerbose(false);
    RUI_NEW_GROUP("App");
    string modeLabels[] = {"SLIT_SCAN", "AUDIO_MAP"};
	RUI_SHARE_ENUM_PARAM(appMode, SLIT_SCAN, AUDIO_MAP, modeLabels);
    RUI_SHARE_PARAM(isDebug);
    RUI_SHARE_PARAM(isPortrait);
    RUI_SHARE_COLOR_PARAM(bgColour);
    
    slitScan.setup();
    audioMapper.setup();
    // Allocate once only using camera capture dimensions
    delayMapFbo.allocate(slitScan.width, slitScan.height);
    allocateScenes();
    
    // OSC Receiver listens for remote control events
    // unlike granular ofxRemoteUI settings, these control high level mode/state changes across multiple clients
    oscReceiver.setup();
    ofAddListener(RemoteEvent::events, this, &ofApp::onRemoteEvent);
    
    // load ofxRemoteUI saved settings, these will override defaults
    RUI_LOAD_FROM_XML();
    
    // show the current mode once loaded from XML
    changeMode(appMode);
    enableDebug(isDebug);
    audioMapper.resetLevels();
}


void ofApp::update(){
    oscReceiver.update();
    // update each scene and draw it if its visible
    // Slit scan
    scenes[0].update();
    if (scenes[0].isVisible) {
        if (slitScan.doAudioMap) {
            audioMapper.update();
            slitScan.audioMap = &audioMapper;
            scenes[1].begin();
            audioMapper.draw();
            scenes[1].end();
            delayMapFbo.begin();
            ofClear(0,0,0,0);
            scenes[1].fbo.draw(0, 0, delayMapFbo.getWidth(), delayMapFbo.getHeight());
            delayMapFbo.end();
            ofPixels pixels;
            delayMapFbo.readToPixels(pixels);
            slitScan.slitScan.setDelayMap(pixels);
        }
        slitScan.update();
        scenes[0].begin();
        if (isPortrait) {
            ofPushMatrix();
            glRotatef(-90, 0, 0, 1);
            glTranslatef(-scenes[0].fbo.getHeight(), 0, 0);
            slitScan.draw(scenes[0].fbo.getHeight(), scenes[0].fbo.getWidth());
            ofPopMatrix();
        }
        else {
            slitScan.draw(scenes[0].fbo.getWidth(), scenes[0].fbo.getHeight());
        }
        scenes[0].end();
    }
    // Audio mapper
    scenes[1].update();
    if (scenes[1].isVisible) {
        if (audioMapper.bg >= AudioMapper::CAM) {
            slitScan.update();
            // need to read in aberration FBO which is the final output from slitscan
            slitScan.aberrationFbo.readToPixels(pixels);
            audioMapper.bgImage.loadData(pixels);
            // This would be faster but doesn't give any of the levels post from slitscan
            //audioMapper.bgImage = &slitScan.cam.getTexture();
        }
        audioMapper.update();
        scenes[1].begin();
        audioMapper.draw();
        scenes[1].end();
    }
}


void ofApp::draw(){
    ofBackground(bgColour);
    // draw both scenes to allow for cross fade transitions
    if (scenes[0].isVisible) scenes[0].draw();
    if (scenes[1].isVisible) scenes[1].draw();
    if (isDebug) {
        stringstream ss;
        ss << "ID:" << id << " " << ofToString(ofGetFrameRate()) << " FPS";
        ofSetColor(160, 160, 255);
        oscReceiver.draw();
        ofDrawBitmapString(ss.str(), ofGetWidth() - 100, ofGetHeight() - 20);
    }
    ofSetColor(255);
}


void ofApp::keyPressed(int key){
    switch (key) {
        case 'f':
            ofToggleFullscreen();
            allocateScenes();
            break;
        default:
            break;
    }
    slitScan.keyPressed(key);
    audioMapper.keyPressed(key);
}

void ofApp::allocateScenes() {
    audioMapper.allocateScenes();
    // Allocates scene FBO to current app width/height, used when window is resized
    scenes[0].resize(ofGetWidth(), ofGetHeight());
    scenes[1].resize(ofGetWidth(), ofGetHeight());
}

void ofApp::enableDebug(bool isDebug){
    RUI_GET_INSTANCE()->setVerbose(isDebug);
    RUI_GET_INSTANCE()->setDrawsNotificationsAutomaticallly(isDebug);
}

void ofApp::changeMode(Mode mode){
    appMode = mode;
    if (appMode == SLIT_SCAN) {
        scenes[0].open();
        scenes[1].close();
    }
    else {
        scenes[1].open();
        scenes[0].close();
    }
}


//////////////////////////////////////////////////////////////////////////////////
// custom event handlers
//////////////////////////////////////////////////////////////////////////////////
void ofApp::onRemoteEvent(RemoteEvent& e){
    if(e.type == RemoteEvent::SWITCH_MODE ) {
        bool idMatch = false;
        ofLogNotice() << "Switch mode for IDs " << e.groupIdsString;
        if (e.groupIdsString != "") {
            vector<string> ids = ofSplitString(e.groupIdsString, "|");
            for (int i=0; i<ids.size(); i++) {
                if (ofToInt(ids[i]) == id) idMatch = true;
            }
        }
        if (idMatch) {
            // do something! just toggle mode for now
            if (appMode == SLIT_SCAN) changeMode(AUDIO_MAP);
            else changeMode(SLIT_SCAN);
        }
        
    }
}

void ofApp::clientDidSomething(RemoteUIServerCallBackArg &arg){
	switch (arg.action) {
		case CLIENT_UPDATED_PARAM:
            ofLogVerbose() << "CLIENT_UPDATED_PARAM: " << arg.paramName << " - " << arg.param.getValueAsString();
            if (arg.paramName == "isDebug") enableDebug(isDebug);
            else if (arg.paramName == "appMode") changeMode(appMode);
			break;
		case CLIENT_DID_SET_PRESET:
            changeMode(appMode);
            enableDebug(isDebug);
            break;
		default:
			break;
	}
}


void ofApp::keyReleased(int key){

}


void ofApp::mouseMoved(int x, int y ){

}


void ofApp::mouseDragged(int x, int y, int button){

}


void ofApp::mousePressed(int x, int y, int button){

}


void ofApp::mouseReleased(int x, int y, int button){

}


void ofApp::windowResized(int w, int h){

}


void ofApp::gotMessage(ofMessage msg){

}


void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
