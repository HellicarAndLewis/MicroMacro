#pragma once

#include "ofMain.h"
#include "SlitScan.h"
#include "AudioMapper.h"
#include "ofxRemoteUIServer.h"

class ofApp : public ofBaseApp{
    
public:
    
    SlitScan slitScan;
    AudioMapper audioMapper;
    
    enum Mode {
        SLIT_SCAN, AUDIO_MAP
    };
    Mode appMode;
    
    ofFbo sceneFbos[2];
    bool isDebug;
    
    void setup();
    void update();
    void draw();
    
    void allocateScenes();
    void enableDebug(bool isDebug);
    
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
