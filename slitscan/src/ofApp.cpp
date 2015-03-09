#include "ofApp.h"


void ofApp::setup(){
    
    // test res
    int width = 640;
    int height = 480;
    
    // quick toggler 0=cam, 1=slitscan, 2=singleslice, 3=doubleslice
    mode = 0;
    cam.setup(width, height, 0.3);
    cam.startCapture();
    
    ofImage distortionMap;
    distortionMap.loadImage("vertical.png");
    // setup slitscan, specify max buffer size
    slitScan.setup(width, height, 30);
    slitScan.setDelayMap(distortionMap);
    // blending means the edges between the scans are feathered
    slitScan.setBlending(true);
    // "time delay is the deepest in history the delay can go
    // and width is the number of frames the distortion will encompass
    // note that the delay cannot be more than the total capacity"
    slitScan.setTimeDelayAndWidth(0, 30);
    
    slicer.setup(width, height);
}


void ofApp::update(){
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    cam.update();
    if (mode > 0) {
        if (cam.isFrameNew) {
            slitScan.addImage(cam.colorImage);
        }
    }
    if (mode > 1) {
        // TODO: optimise this using pointer instead of setting image? Or draw into FBO?
        slicer.srcImg = slitScan.getOutputImage();
        slicer.update();
        //slitScan.getOutputImage().draw(0, 0);
    }
}


void ofApp::draw(){
    //cam.drawFlow(ofGetWidth(), ofGetHeight());
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
            slicer.draw(-10);
            slicer.draw(10);
            break;
        default:
            break;
    }
    
    stringstream ss;
    ss << "press m to toggle MODE: " << mode;
    ofDrawBitmapString(ss.str(), 0, ofGetHeight() - 20);
}


void ofApp::keyPressed(int key){
    switch (key) {
        case 'm':
            mode++;
            if (mode > 3) mode = 0;
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
