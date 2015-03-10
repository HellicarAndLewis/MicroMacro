#include "ofApp.h"


void ofApp::setup(){
    
    // defaults
    sliceThickness = 20;
    sliceOffset = 10;
    slitScanTimeWidth = 30;
    slitScanTimeDelay = 0;
    
    // test res using iSight
    int width = 640;
    int height = 480;
    cam.setup(width, height, 0.3);
    //cam.useVideoPlayer = true;
    cam.startCapture();
    
    // slit scan setup
    // setup slitscan, specify max buffer size
    slitScan.setup(width, height, 120);
    // blending means the edges between the scans are feathered
    slitScan.setBlending(true);
    // "time delay is the deepest in history the delay can go
    // and width is the number of frames the distortion will encompass
    // note that the delay cannot be more than the total capacity"
    slitScan.setTimeDelayAndWidth(slitScanTimeDelay, slitScanTimeWidth);
    // delay maps
	sampleMapStrings.push_back("maps/down_to_up.png");
    sampleMapStrings.push_back("maps/left_to_right.png");
	sampleMapStrings.push_back("maps/soft_noise.png");
	sampleMapStrings.push_back("maps/random_grid.png");
	sampleMapStrings.push_back("maps/up_to_down.png");
	sampleMapStrings.push_back("maps/right_to_left.png");
	for(int i = 0; i < sampleMapStrings.size(); i++){
		ofImage* map = new ofImage();
		map->allocate(width, height, OF_IMAGE_GRAYSCALE);
		map->loadImage(sampleMapStrings[i]);
		sampleMaps.push_back( map );
	}
    slitScan.setDelayMap(*(sampleMaps[currentSampleMapIndex = 0]));
    
    // give slicer a width and height to work with
    slicer.setup(width, height);
    
    aberrationShader.load("shaders/aberration.vert", "shaders/aberration.frag");
    aberrationFbo.allocate(width, height);
    aberrationFbo.begin();
    ofClear(0, 0, 0);
    aberrationFbo.end();
    
    // Using ofxRemoteUI https://github.com/armadillu/ofxRemoteUI/
    // optionaly specify port here, otherwise random
	RUI_SETUP();
	ofAddListener(RUI_GET_OF_EVENT(), this, &ofApp::clientDidSomething);
	RUI_GET_INSTANCE()->setVerbose(true);
    // share controls
    string modeLabels[] = {"CAM", "SLIT_SCAN", "SLICE_SINGLE", "SLICE_DOUBLE"};
	RUI_SHARE_ENUM_PARAM(mode, CAM, SLICE_DOUBLE, modeLabels);
	RUI_NEW_GROUP("Slicer");
	RUI_SHARE_PARAM(sliceThickness, 0, 30 );
	RUI_SHARE_PARAM(sliceOffset, 0, 30 );
    RUI_NEW_GROUP("Slit Scan");
	RUI_SHARE_PARAM(slitScanTimeWidth, 0, 120);
	RUI_SHARE_PARAM(slitScanTimeDelay, 0, 120);
    RUI_SHARE_PARAM(currentSampleMapIndex, 0, sampleMapStrings.size()-1);
    
    RUI_NEW_GROUP("Aberration");
	RUI_SHARE_PARAM(aberrationROffset.x, 0.0, 50.0);
    
}


void ofApp::update(){
    cam.update();
    if (mode > 0) {
        // all modes above 0 use slitscan
        if (cam.isFrameNew) {
            slitScan.addImage(cam.colorImage);
        }
    }
    if (mode > 1) {
        // all modes above 1 use slicer
        slicer.begin();
        slitScan.getOutputImage().draw(0, 0);
        slicer.end();
    }
    
    aberrationFbo.begin();
    switch (mode) {
        case 0:
            cam.draw(0, 0);
            break;
        case 1:
            slitScan.getOutputImage().draw(0, 0);
            break;
        case 2:
            cam.draw(0, 0);
            slicer.draw(0);
            break;
        case 3:
            slicer.draw(-sliceOffset);
            slicer.draw(sliceOffset);
            break;
        default:
            break;
    }
    aberrationFbo.end();
}


void ofApp::draw(){
    //cam.drawFlow(ofGetWidth(), ofGetHeight());
    
    aberrationShader.begin();
    aberrationFbo.getTextureReference().bind();
    //shader.setUniformTexture("maskTex", maskFbo.getTextureReference(), 1 );
    //aberrationShader.setUniform1i("tex0", 0);
    aberrationShader.setUniform2f("rOffset", aberrationROffset.x, 0.0);
    aberrationShader.setUniform2f("gOffset", 0.0, 0.0);
    aberrationShader.setUniform2f("bOffset", 0.0, 0.0);
    float w = 640;
    float h = 480;
	// draw quads
	glBegin(GL_QUADS);
	glMultiTexCoord2f(GL_TEXTURE0, 0.0f, h);
	glVertex3f(0, h, 0);
	glMultiTexCoord2f(GL_TEXTURE0, 0.0f, 0.0f);
	glVertex3f(0, 0, 0);
	glMultiTexCoord2f(GL_TEXTURE0, w, 0.0f);
	glVertex3f(w, 0, 0);
	glMultiTexCoord2f(GL_TEXTURE0, w, h);
	glVertex3f(w, h, 0);
	glEnd();
	// unbind the textures
    aberrationFbo.getTextureReference().unbind();
    aberrationShader.end();
    
    stringstream ss;
    ss << "MODE: " << mode;
    ss << "\n" << ofToString(ofGetFrameRate()) << " FPS";
    ofSetColor(0, 0, 200);
    ofDrawBitmapString(ss.str(), ofGetWidth() - 100, ofGetHeight() - 20);
    ofSetColor(255);
}


void ofApp::keyPressed(int key){
}

void ofApp::clientDidSomething(RemoteUIServerCallBackArg &arg){
    
	switch (arg.action) {
		case CLIENT_CONNECTED: ofLogNotice() << "CLIENT_CONNECTED" << endl; break;
		case CLIENT_DISCONNECTED: ofLogNotice() << "CLIENT_DISCONNECTED" << endl; break;
		case CLIENT_UPDATED_PARAM: ofLogVerbose() << "CLIENT_UPDATED_PARAM: " << arg.paramName << " - " << arg.param.getValueAsString();
            if (arg.paramName == "slitScanTimeWidth") slitScan.setTimeWidth(slitScanTimeWidth);
            if (arg.paramName == "slitScanTimeDelay") slitScan.setTimeDelay(slitScanTimeDelay);
            if (arg.paramName == "sliceThickness") slicer.setThickness(sliceThickness);
            if (arg.paramName == "currentSampleMapIndex") slitScan.setDelayMap(*(sampleMaps[currentSampleMapIndex]));
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
