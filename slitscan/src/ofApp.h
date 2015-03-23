#pragma once

#include "ofMain.h"
#include "SlitScan.h"
#include "AudioMapper.h"
#include "ofxRemoteUIServer.h"
#include "OscReceiver.h"

// TODO: add quad warper to match projection surface?
// TODO: add portrait mode support
// TODO: more complexity in audio reactive mode
class Scene {
public:
    ofFbo fbo;
    float alpha, targetAlpha;
    bool isVisible;
    Scene() {
        alpha = targetAlpha = 0;
        isVisible = false;
    }
    void open() {
        targetAlpha = 1;
    }
    void close() {
        targetAlpha = 0;
    }
    void update() {
        alpha = ofLerp(alpha, targetAlpha, 0.2);
        if (alpha < 0.001) {
            isVisible = false;
        } else {
            isVisible = true;
        }
    }
    void draw() {
        ofSetColor(alpha*255, 255, 255, alpha*255);
        fbo.draw(0,0);
        ofSetColor(255);
    }
    void begin() {
        fbo.begin();
        ofClear(0,0,0,0);
    }
    void end() {
        fbo.end();
    }
    void resize(int width, int height) {
        fbo.allocate(width, height);
        begin();
        end();
    }
};

class ofApp : public ofBaseApp{
    
public:
    int id;
    ofColor bgColour;
    
    SlitScan slitScan;
    AudioMapper audioMapper;
    
    enum Mode {
        SLIT_SCAN, AUDIO_MAP
    };
    Mode appMode;
    
    Scene scenes[2];
    bool isDebug;
    
    OscReceiver oscReceiver;
    
    void setup();
    void update();
    void draw();
    
    void allocateScenes();
    void enableDebug(bool isDebug);
    void changeMode(Mode mode);
    
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
    void onRemoteEvent(RemoteEvent& e);
};
