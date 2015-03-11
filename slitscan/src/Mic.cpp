//
//  Mic.cpp
//  slitscan
//
//  Created by Chris on 11/03/2015.
//
//

#include "Mic.h"

void Mic::setup(){
	
	// 0 output channels,
	// 2 input channels
	// 44100 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)
	soundStream.listDevices();
	
	// Optionally specify device ID? Shouldn't need to
	//soundStream.setDeviceID(0);
	int bufferSize = 256;
	
	left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
	volHistory.assign(400, 0.0);
	
	bufferCounter = 0;
	drawCounter = 0;
	smoothedVol = 0.0;
	scaledVol = 0.0;
	soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
    
}


void Mic::update(){
	// scaled volume is 0-1
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
	volHistory.push_back( scaledVol );
	if( volHistory.size() >= 400 ){
		volHistory.erase(volHistory.begin(), volHistory.begin()+1);
	}
}


void Mic::draw(){
}


void Mic::audioIn(float * input, int bufferSize, int nChannels){
	
	float curVol = 0.0;
	int numCounted = 0;
    
	// calculate volume using average of all left/right values
	for (int i = 0; i < bufferSize; i++){
		left[i] = input[i*2]*0.5;
		right[i]	 = input[i*2+1]*0.5;
		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted+=2;
	}
	curVol /= (float)numCounted;
	curVol = sqrt( curVol );
	
	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;
	
	bufferCounter++;
}

void Mic::start(){
    soundStream.start();
}
void Mic::stop(){
    soundStream.stop();
}


void Mic::keyPressed  (int key){
	if( key == 's' ){
		soundStream.start();
	}
	
	if( key == 'e' ){
		soundStream.stop();
	}
}


void Mic::keyReleased(int key){
	
}


void Mic::mouseMoved(int x, int y ){
	
}


void Mic::mouseDragged(int x, int y, int button){
	
}


void Mic::mousePressed(int x, int y, int button){
	
}


void Mic::mouseReleased(int x, int y, int button){
    
}


void Mic::windowResized(int w, int h){
    
}


void Mic::gotMessage(ofMessage msg){
    
}


void Mic::dragEvent(ofDragInfo dragInfo){ 
    
}