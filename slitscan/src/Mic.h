//
//  Mic.h
//  slitscan
//
//  Created by Chris on 11/03/2015.
//
//

#pragma once
#include "ofMain.h"
#include "ofxFFTLive.h"
#include "ofxFFTFile.h"

class Mic : public ofBaseApp{
	
public:
    
    void setup();
    void update();
    void draw();
    
    void start();
    void stop();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void audioIn(float * input, int bufferSize, int nChannels);
	
    vector <float> left;
    vector <float> right;
    vector <float> volHistory;
    
    int bufferCounter;
    int drawCounter;
    float smoothedVol;
    float scaledVol;
    
    ofSoundStream soundStream;
    
    ofSoundPlayer soundPlayer;
    ofxFFTLive fftLive;
    ofxFFTFile fftFile;
    ofMesh meshOriginal;
    ofMesh meshWarped;
    ofEasyCam cam;
};
