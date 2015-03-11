//
//  SlitScan.h
//  slitscan
//
//  Created by Chris on 11/03/2015.
//
//

#pragma once

#include "ofMain.h"
#include "Cam.h"
#include "ofxSlitScan.h"
#include "Slicer.h"
#include "ofxRemoteUIServer.h"

class SlitScan {
    
public:
    // Cam class handles switching between blackmagic input and standard video grabber
    // It also generates optical flow textures and data
    Cam cam;
    // slit scan effect addon
    ofxSlitScan slitScan;
    int slitScanTimeWidth, slitScanTimeDelay;
    vector<string> sampleMapStrings;
	vector<ofImage*> sampleMaps;
	int currentSampleMapIndex;
    
    // Slicer chops up an image/texture into vertical strips and draw the output into an FBO
    Slicer slicer;
    int sliceThickness, sliceOffset;
    
    // Simple mode switcher whilst testing
    enum Mode {
        CAM=0, SLIT_SCAN, SLICE_SINGLE, SLICE_DOUBLE
    };
    Mode mode;
    
    ofFbo aberrationFbo;
    ofShader aberrationShader;
    ofVec2f aberrationROffset;
    
    int width, height;
    
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void clientDidSomething(RemoteUIServerCallBackArg & arg);
};
