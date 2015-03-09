#pragma once

#include "ofMain.h"
#include "Cam.h"
#include "ofxSlitScan.h"
#include "Slicer.h"

class ofApp : public ofBaseApp{
    
public:
    // Cam class handles switching between blackmagic input and standard video grabber
    // It also generates optical flow textures and data
    Cam cam;
    // slit scan effect addon
    ofxSlitScan slitScan;
    // Slicer chops up an image/texture into vertical strips and draw the output into an FBO
    Slicer slicer;
    // quick thing to toggle modes: will be replaced by GUI
    int mode;
    
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
    
};
