//
//  AudioMapper.h
//  slitscan
//
//  Created by Chris on 11/03/2015.
//
//

#pragma once
#include "Mic.h"
#include "ofxRemoteUIServer.h"
#include "AlphaMask.h"

// Takes audio input and visualises it in different ways
class AudioMapper {
public:
    
    // visual states
    enum Layout {
        LEFT_RIGHT=0, RIGHT_LEFT, UP_DOWN, DOWN_UP, MIRROR_SIDE_V, MIRROR_SIDE_H, MIRROR_CENTRE_V, MIRROR_CENTRE_H, SOLID_V, SOLID_H
    };
    Layout layout;
    bool isFadeOn;
    bool isScaleOn;
    
    enum BgType {
        GREYSCALE=0, GREYSCALE_NOISE, CAM, CAM_SLICE_V, CAM_SLICE_H
    };
    BgType bg;
    
    bool usePerlin;
    
    // mic provides actual sound input
    Mic mic;
    
    // sound input is mapped to visual levels
    // number of levels is determined directly
    // or by specifying a per level thickness and gap
    vector<float> levels;
    int levelCount;
    bool useLevelCount;
    int width;
    int height;
    float thick;
    float gap;
    float peakToLengthRatio;
    ofColor colour;
    
    AlphaMask alphaMask;
    ofImage * bgImage;
    ofFbo bgFbo;
    
    // audio sample settings
    float mapMin, mapMax;
    float audioThreshold;
    float audioPeakDecay;
    float audioMaxDecay;
    bool audioMirror;
    
    void setup();
    void update();
    void draw();
    void drawBars(Layout layout);
    void resetLevels();
    void allocateScenes();
    bool getIsLayoutVertical();
    void clientDidSomething(RemoteUIServerCallBackArg & arg);
    
    void keyPressed(int key);
    
    
};