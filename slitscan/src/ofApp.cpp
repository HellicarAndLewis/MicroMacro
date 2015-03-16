#include "ofApp.h"


void ofApp::setup(){
    
    ofLogNotice() << "App " << id << " setup";
    
    isDebug = true;
    bgColour.set(0);
    
    // Using ofxRemoteUI https://github.com/armadillu/ofxRemoteUI/
    // optionaly specify port here, otherwise random
	RUI_SETUP(4000);
	ofAddListener(RUI_GET_OF_EVENT(), this, &ofApp::clientDidSomething);
	RUI_GET_INSTANCE()->setVerbose(isDebug);
    RUI_NEW_GROUP("App");
    string modeLabels[] = {"SLIT_SCAN", "AUDIO_MAP"};
	RUI_SHARE_ENUM_PARAM(appMode, SLIT_SCAN, AUDIO_MAP, modeLabels);
    RUI_SHARE_PARAM(isDebug);
    RUI_SHARE_COLOR_PARAM(bgColour);
    
    appMode == SLIT_SCAN;
    slitScan.setup();
    audioMapper.setup();
    allocateScenes();
    
    // OSC Receiver listens for remote control events
    // unlike granular ofxRemoteUI settings, these control high level mode/state changes across multiple clients
    oscReceiver.setup();
    ofAddListener(RemoteEvent::events, this, &ofApp::onRemoteEvent);
    
    RUI_LOAD_FROM_XML();
}


void ofApp::update(){
    oscReceiver.update();
    if (appMode == SLIT_SCAN) {
        slitScan.update();
        sceneFbos[0].begin();
        slitScan.draw(sceneFbos[0].getWidth(), sceneFbos[0].getHeight());
        sceneFbos[0].end();
    }
    else {
        audioMapper.update();
        sceneFbos[0].begin();
        ofClear(0,0,0,0);
        audioMapper.draw();
        sceneFbos[0].end();
    }
}


void ofApp::draw(){
    ofBackground(bgColour);
    sceneFbos[0].draw(0, 0);
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
}

void ofApp::allocateScenes() {
    audioMapper.resetLevels();
    // Allocates scene FBO to current app width/height, used when window is resized
    sceneFbos[0].allocate(ofGetWidth(), ofGetHeight());
    sceneFbos[0].begin();
    ofClear(0, 0, 0);
    sceneFbos[0].end();
}

void ofApp::enableDebug(bool isDebug){
    RUI_GET_INSTANCE()->setVerbose(isDebug);
    RUI_GET_INSTANCE()->setDrawsNotificationsAutomaticallly(isDebug);
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
            if (appMode == SLIT_SCAN) appMode = AUDIO_MAP;
            else appMode = SLIT_SCAN;
        }
        
    }
}

void ofApp::clientDidSomething(RemoteUIServerCallBackArg &arg){
    
	switch (arg.action) {
		case CLIENT_CONNECTED: ofLogNotice() << "CLIENT_CONNECTED" << endl; break;
		case CLIENT_DISCONNECTED: ofLogNotice() << "CLIENT_DISCONNECTED" << endl; break;
		case CLIENT_UPDATED_PARAM:
            ofLogVerbose() << "CLIENT_UPDATED_PARAM: " << arg.paramName << " - " << arg.param.getValueAsString();
            if (arg.paramName == "isDebug") enableDebug(isDebug);
			break;
		case CLIENT_DID_SET_PRESET: ofLogNotice() << "CLIENT_DID_SET_PRESET" << endl; break;
		case CLIENT_SAVED_PRESET: ofLogNotice() << "CLIENT_SAVED_PRESET" << endl; break;
		case CLIENT_DELETED_PRESET: ofLogNotice() << "CLIENT_DELETED_PRESET" << endl; break;
		case CLIENT_SAVED_STATE: ofLogNotice() << "CLIENT_SAVED_STATE" << endl; break;
		case CLIENT_DID_RESET_TO_XML: ofLogNotice() << "CLIENT_DID_RESET_TO_XML" << endl; break;
		case CLIENT_DID_RESET_TO_DEFAULTS: ofLogNotice() << "CLIENT_DID_RESET_TO_DEFAULTS" << endl; break;
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
